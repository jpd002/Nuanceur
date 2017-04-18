#include <cstring>
#include "nuanceur/generators/SpirvShaderGenerator.h"

using namespace Nuanceur;

CSpirvShaderGenerator::CSpirvShaderGenerator(Framework::CStream& outputStream, const CShaderBuilder& shaderBuilder, SHADER_TYPE shaderType)
: m_outputStream(outputStream)
, m_shaderBuilder(shaderBuilder)
, m_shaderType(shaderType)
{

}

void CSpirvShaderGenerator::Generate(Framework::CStream& outputStream, const CShaderBuilder& shaderBuilder, SHADER_TYPE shaderType)
{
	CSpirvShaderGenerator generator(outputStream, shaderBuilder, shaderType);
	generator.Generate();
}

static bool IsIdentitySwizzle(SWIZZLE_TYPE swizzle)
{
	return (swizzle == SWIZZLE_X)   ||
	       (swizzle == SWIZZLE_XY)  ||
	       (swizzle == SWIZZLE_XYZ) ||
	       (swizzle == SWIZZLE_XYZW);
}

void CSpirvShaderGenerator::Generate()
{
	//Some notes:
	//Mali drivers seem to require a struct to be used for output vertex params

	Write32(spv::MagicNumber);
	Write32(spv::Version);     //This needs to be 0x10000
	Write32(0);                //Generator
	Write32(0);                //Bound
	Write32(0);                //Instruction Schema

	m_hasUniforms = std::count_if(m_shaderBuilder.GetSymbols().begin(), m_shaderBuilder.GetSymbols().end(),
		[] (const CShaderBuilder::SYMBOL& symbol) { return symbol.location == CShaderBuilder::SYMBOL_LOCATION_UNIFORM; }) != 0;

	m_hasTextures = std::count_if(m_shaderBuilder.GetSymbols().begin(), m_shaderBuilder.GetSymbols().end(),
		[] (const CShaderBuilder::SYMBOL& symbol) { return symbol.location == CShaderBuilder::SYMBOL_LOCATION_TEXTURE; }) != 0;

	auto voidTypeId = AllocateId();
	auto mainFunctionTypeId = AllocateId();
	m_floatTypeId = AllocateId();
	m_float4TypeId = AllocateId();
	m_matrix44TypeId = AllocateId();
	auto int32TypeId = AllocateId();
	auto perVertexStructTypeId = AllocateId();
	m_inputFloat4PointerTypeId = AllocateId();
	m_outputFloat4PointerTypeId = AllocateId();
	auto outputPerVertexStructPointerTypeId = AllocateId();

	if(m_hasUniforms)
	{
		m_uniformStructTypeId = AllocateId();
		m_pushUniformStructPointerTypeId = AllocateId();
		m_pushFloat4PointerTypeId = AllocateId();
		m_pushMatrix44PointerTypeId = AllocateId();
		m_pushUniformVariableId = AllocateId();
	}

	if(m_hasTextures)
	{
		m_image2DTypeId = AllocateId();
		m_imageSamplerTypeId = AllocateId();
		m_imageSamplerPointerTypeId = AllocateId();
		AllocateTextureIds();
	}

	AllocateInputPointerIds();
	AllocateOutputPointerIds();

	m_outputPerVertexVariableId = AllocateId();

	auto mainFunctionId = AllocateId();
	auto mainFunctionLabelId = AllocateId();

	WriteOp(spv::OpCapability, spv::CapabilityShader);
	WriteOp(spv::OpMemoryModel, spv::AddressingModelLogical, spv::MemoryModelGLSL450);

	//Write Entry Point
	{
		auto executionModel = spv::ExecutionModelVertex;
		switch(m_shaderType)
		{
		case SHADER_TYPE_VERTEX:
			executionModel = spv::ExecutionModelVertex;
			break;
		case SHADER_TYPE_FRAGMENT:
			executionModel = spv::ExecutionModelFragment;
			break;
		default:
			assert(false);
			break;
		}

		std::vector<uint32> inputPointerIds;
		inputPointerIds.reserve(m_inputPointerIds.size());
		for(const auto& inputPointerIdPair : m_inputPointerIds)
		{
			inputPointerIds.push_back(inputPointerIdPair.second);
		}

		std::vector<uint32> outputPointerIds;
		outputPointerIds.reserve(m_outputPointerIds.size() + 1);
		if(m_shaderType == SHADER_TYPE_VERTEX)
		{
			outputPointerIds.push_back(m_outputPerVertexVariableId);
		}
		for(const auto& outputPointerIdPair : m_outputPointerIds)
		{
			outputPointerIds.push_back(outputPointerIdPair.second);
		}

		WriteOp(spv::OpEntryPoint,
			executionModel, mainFunctionId, "main",
			inputPointerIds, outputPointerIds
		);
	}

	if(m_shaderType == SHADER_TYPE_FRAGMENT)
	{
		WriteOp(spv::OpExecutionMode, mainFunctionId, spv::ExecutionModeOriginUpperLeft);
	}

	//Names
	//WriteOp(spv::OpSource, spv::SourceLanguageUnknown, 100);
	//WriteOp(spv::OpName, mainFunctionId, "main");

	//Annotations
	if(m_shaderType == SHADER_TYPE_VERTEX)
	{
		WriteOp(spv::OpMemberDecorate, perVertexStructTypeId, 0, spv::DecorationBuiltIn, spv::BuiltInPosition);
		WriteOp(spv::OpMemberDecorate, perVertexStructTypeId, 1, spv::DecorationBuiltIn, spv::BuiltInPointSize);
		WriteOp(spv::OpDecorate, perVertexStructTypeId, spv::DecorationBlock);
		m_intConstants.insert(0);    //Will be required when accessing built-in position
	}

	if(m_hasUniforms)
	{
		DecorateUniformStructIds();
	}

	if(m_hasTextures)
	{
		DecorateTextureIds();
	}

	DecorateInputPointerIds();
	DecorateOutputPointerIds();

	//Type declarations
	WriteOp(spv::OpTypeVoid, voidTypeId);
	WriteOp(spv::OpTypeFunction, mainFunctionTypeId, voidTypeId);
	WriteOp(spv::OpTypeFloat, m_floatTypeId, 32);
	WriteOp(spv::OpTypeVector, m_float4TypeId, m_floatTypeId, 4);
	WriteOp(spv::OpTypeMatrix, m_matrix44TypeId, m_float4TypeId, 4);
	WriteOp(spv::OpTypeInt, int32TypeId, 32, 1);
	WriteOp(spv::OpTypePointer, m_inputFloat4PointerTypeId, spv::StorageClassInput, m_float4TypeId);
	WriteOp(spv::OpTypePointer, m_outputFloat4PointerTypeId, spv::StorageClassOutput, m_float4TypeId);
	
	if(m_shaderType == SHADER_TYPE_VERTEX)
	{
		WriteOp(spv::OpTypeStruct, perVertexStructTypeId, m_float4TypeId, m_floatTypeId);
		WriteOp(spv::OpTypePointer, outputPerVertexStructPointerTypeId, spv::StorageClassOutput, perVertexStructTypeId);
	}

	if(m_hasUniforms)
	{
		DeclareUniformStructIds();
		WriteOp(spv::OpTypePointer, m_pushUniformStructPointerTypeId, spv::StorageClassPushConstant, m_uniformStructTypeId);
		WriteOp(spv::OpTypePointer, m_pushFloat4PointerTypeId, spv::StorageClassPushConstant, m_float4TypeId);
		WriteOp(spv::OpTypePointer, m_pushMatrix44PointerTypeId, spv::StorageClassPushConstant, m_matrix44TypeId);
	}

	if(m_hasTextures)
	{
		WriteOp(spv::OpTypeImage, m_image2DTypeId, m_float4TypeId, spv::Dim2D, 0, 0, 0, 1, spv::ImageFormatUnknown);
		WriteOp(spv::OpTypeSampledImage, m_imageSamplerTypeId, m_image2DTypeId);
		WriteOp(spv::OpTypePointer, m_imageSamplerPointerTypeId, spv::StorageClassUniformConstant, m_imageSamplerTypeId);
	}

	DeclareInputPointerIds();
	DeclareOutputPointerIds();

	//Declare Output PerVertex (Position + PointSize) BuiltIn
	if(m_shaderType == SHADER_TYPE_VERTEX)
	{
		WriteOp(spv::OpVariable, outputPerVertexStructPointerTypeId, m_outputPerVertexVariableId, spv::StorageClassOutput);
	}

	if(m_hasUniforms)
	{
		WriteOp(spv::OpVariable, m_pushUniformStructPointerTypeId, m_pushUniformVariableId, spv::StorageClassPushConstant);
	}

	if(m_hasTextures)
	{
		DeclareTextureIds();
	}

	//Allocate and Declare Constants
	for(const auto& intConstant : m_intConstants)
	{
		auto id = AllocateId();
		WriteOp(spv::OpConstant, int32TypeId, id,intConstant);
		m_intConstantIds[intConstant] = id;
	}

	DeclareTemporaryValueIds();

	//Write main function
	{
		WriteOp(spv::OpFunction, voidTypeId, mainFunctionId, spv::FunctionControlMaskNone, mainFunctionTypeId);
		WriteOp(spv::OpLabel, mainFunctionLabelId);

		for(const auto& statement : m_shaderBuilder.GetStatements())
		{
			const auto& dstRef = statement.dstRef;
			const auto& src1Ref = statement.src1Ref;
			const auto& src2Ref = statement.src2Ref;
			switch(statement.op)
			{
			case CShaderBuilder::STATEMENT_OP_ADD:
				{
					auto src1Id = LoadFromSymbol(src1Ref);
					auto src2Id = LoadFromSymbol(src2Ref);
					auto resultId = AllocateId();
					WriteOp(spv::OpFAdd, m_float4TypeId, resultId, src1Id, src2Id);
					StoreToSymbol(dstRef, resultId);
				}
				break;
			case CShaderBuilder::STATEMENT_OP_MULTIPLY:
				{
					assert(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX);
					auto src1Id = LoadFromSymbol(src1Ref);
					auto src2Id = LoadFromSymbol(src2Ref);
					auto resultId = AllocateId();
					WriteOp(spv::OpMatrixTimesVector, m_float4TypeId, resultId, src1Id, src2Id);
					StoreToSymbol(dstRef, resultId);
				}
				break;
			case CShaderBuilder::STATEMENT_OP_SAMPLE:
				{
					auto src1Id = LoadFromSymbol(src1Ref);
					auto src2Id = LoadFromSymbol(src2Ref);
					auto resultId = AllocateId();
					WriteOp(spv::OpImageSampleImplicitLod, m_float4TypeId, resultId, src1Id, src2Id);
					StoreToSymbol(dstRef, resultId);
				}
				break;
			case CShaderBuilder::STATEMENT_OP_NEWVECTOR4:
				{
					assert(statement.GetSourceCount() == 2);
					assert(statement.src1Ref.swizzle == SWIZZLE_XYZ);
					assert(statement.src2Ref.swizzle == SWIZZLE_X);
					auto src1Id = LoadFromSymbol(src1Ref);
					auto src2Id = LoadFromSymbol(src2Ref);
					auto elemXId = AllocateId();
					auto elemYId = AllocateId();
					auto elemZId = AllocateId();
					auto elemWId = AllocateId();
					auto resultId = AllocateId();
					WriteOp(spv::OpCompositeExtract, m_floatTypeId, elemXId, src1Id, 0);
					WriteOp(spv::OpCompositeExtract, m_floatTypeId, elemYId, src1Id, 1);
					WriteOp(spv::OpCompositeExtract, m_floatTypeId, elemZId, src1Id, 2);
					WriteOp(spv::OpCompositeExtract, m_floatTypeId, elemWId, src2Id, 0);
					WriteOp(spv::OpCompositeConstruct, m_float4TypeId, resultId, elemXId, elemYId, elemZId, elemWId);
					StoreToSymbol(dstRef, resultId);
				}
				break;
			case CShaderBuilder::STATEMENT_OP_ASSIGN:
				{
					auto src1Id = LoadFromSymbol(src1Ref);
					StoreToSymbol(dstRef, src1Id);
				}
				break;
			default:
				assert(false);
				break;
			}
		}

		WriteOp(spv::OpReturn);
		WriteOp(spv::OpFunctionEnd);
	}

	//Patch in bound
	m_outputStream.Seek(12, Framework::STREAM_SEEK_SET);
	m_outputStream.Write32(m_nextId);

	m_outputStream.Seek(0, Framework::STREAM_SEEK_END);
}

void CSpirvShaderGenerator::AllocateInputPointerIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_INPUT) continue;
//		auto semantic = m_shaderBuilder.GetInputSemantic(symbol);
		assert(symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4);
		assert(m_inputPointerIds.find(symbol.index) == std::end(m_inputPointerIds));
		auto pointerId = AllocateId();
		m_inputPointerIds[symbol.index] = pointerId;
	}
}

void CSpirvShaderGenerator::DecorateInputPointerIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_INPUT) continue;
		auto semantic = m_shaderBuilder.GetInputSemantic(symbol);
		assert(m_inputPointerIds.find(symbol.index) != std::end(m_inputPointerIds));
		auto location = MapSemanticToLocation(semantic.type, semantic.index);
		auto pointerId = m_inputPointerIds[symbol.index];
		WriteOp(spv::OpDecorate, pointerId, spv::DecorationLocation, location);
	}
}

void CSpirvShaderGenerator::DeclareInputPointerIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_INPUT) continue;
//		auto semantic = m_shaderBuilder.GetInputSemantic(symbol);
		assert(m_inputPointerIds.find(symbol.index) != std::end(m_inputPointerIds));
		auto pointerId = m_inputPointerIds[symbol.index];
		WriteOp(spv::OpVariable, m_inputFloat4PointerTypeId, pointerId, spv::StorageClassInput);
	}
}

void CSpirvShaderGenerator::AllocateOutputPointerIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_OUTPUT) continue;
		auto semantic = m_shaderBuilder.GetOutputSemantic(symbol);
		//Don't bother with position for now
		if(semantic.type == Nuanceur::SEMANTIC_SYSTEM_POSITION) continue;
		assert(symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4);
		assert(m_outputPointerIds.find(symbol.index) == std::end(m_outputPointerIds));
		auto pointerId = AllocateId();
		m_outputPointerIds[symbol.index] = pointerId;
	}
}

void CSpirvShaderGenerator::DecorateOutputPointerIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_OUTPUT) continue;
		auto semantic = m_shaderBuilder.GetOutputSemantic(symbol);
		//Don't bother with position for now
		if(semantic.type == Nuanceur::SEMANTIC_SYSTEM_POSITION) continue;
		assert(m_outputPointerIds.find(symbol.index) != std::end(m_outputPointerIds));
		auto pointerId = m_outputPointerIds[symbol.index];
		auto location = MapSemanticToLocation(semantic.type, semantic.index);
		WriteOp(spv::OpDecorate, pointerId, spv::DecorationLocation, location);
	}
}

void CSpirvShaderGenerator::DeclareOutputPointerIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_OUTPUT) continue;
		auto semantic = m_shaderBuilder.GetOutputSemantic(symbol);
		//Don't bother with position for now
		if(semantic.type == Nuanceur::SEMANTIC_SYSTEM_POSITION) continue;
		assert(m_outputPointerIds.find(symbol.index) != std::end(m_outputPointerIds));
		auto pointerId = m_outputPointerIds[symbol.index];
		WriteOp(spv::OpVariable, m_outputFloat4PointerTypeId, pointerId, spv::StorageClassOutput);
	}
}

void CSpirvShaderGenerator::DeclareTemporaryValueIds()
{
	std::map<float, uint32> floatConstantIds;
	auto registerFloatConstant =
		[this, &floatConstantIds](float value)
		{
			if(floatConstantIds.find(value) != std::end(floatConstantIds)) return;
			floatConstantIds[value] = AllocateId();
		};

	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEMPORARY) continue;
		auto temporaryValue = m_shaderBuilder.GetTemporaryValue(symbol);
		registerFloatConstant(temporaryValue.x);
		registerFloatConstant(temporaryValue.y);
		registerFloatConstant(temporaryValue.z);
		registerFloatConstant(temporaryValue.w);
	}

	for(const auto& floatConstantIdPair : floatConstantIds)
	{
		WriteOp(spv::OpConstant, m_floatTypeId, floatConstantIdPair.second, floatConstantIdPair.first);
	}

	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEMPORARY) continue;
		auto temporaryValue = m_shaderBuilder.GetTemporaryValue(symbol);
		uint32 temporaryValueId = AllocateId();
		uint32 valueXId = floatConstantIds[temporaryValue.x];
		uint32 valueYId = floatConstantIds[temporaryValue.y];
		uint32 valueZId = floatConstantIds[temporaryValue.z];
		uint32 valueWId = floatConstantIds[temporaryValue.w];
		WriteOp(spv::OpConstantComposite, m_float4TypeId, temporaryValueId, valueXId, valueYId, valueZId, valueWId);
		m_temporaryValueIds[symbol.index] = temporaryValueId;
	}
}

void CSpirvShaderGenerator::DecorateUniformStructIds()
{
	assert(m_hasUniforms);

	uint32 currentOffset = 0;
	unsigned int memberIndex = 0;
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
		WriteOp(spv::OpMemberDecorate, m_uniformStructTypeId, memberIndex, spv::DecorationOffset, currentOffset);
		if(symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX)
		{
			WriteOp(spv::OpMemberDecorate, m_uniformStructTypeId, memberIndex, spv::DecorationColMajor);
			WriteOp(spv::OpMemberDecorate, m_uniformStructTypeId, memberIndex, spv::DecorationMatrixStride, 16);
		}
		m_uniformStructMemberIndices[symbol.index] = memberIndex;
		m_intConstants.insert(memberIndex);
		//Assuming std430 layout
		switch(symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			//sizeof(float) * 4
			currentOffset += 16;
			break;
		case CShaderBuilder::SYMBOL_TYPE_MATRIX:
			//sizeof(float) * 16
			currentOffset += 64;
			break;
		}
		memberIndex++;
	}

	WriteOp(spv::OpDecorate, m_uniformStructTypeId, spv::DecorationBlock);
}

void CSpirvShaderGenerator::DeclareUniformStructIds()
{
	assert(m_hasUniforms);

	std::vector<uint32> structComponents;
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
		switch(symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			structComponents.push_back(m_float4TypeId);
			break;
		case CShaderBuilder::SYMBOL_TYPE_MATRIX:
			structComponents.push_back(m_matrix44TypeId);
			break;
		default:
			assert(false);
			break;
		}
	}
	
	assert(m_uniformStructTypeId != 0);
	WriteOp(spv::OpTypeStruct, m_uniformStructTypeId, structComponents);
}

void CSpirvShaderGenerator::AllocateTextureIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEXTURE) continue;
		assert(symbol.type == CShaderBuilder::SYMBOL_TYPE_TEXTURE2D);
		assert(m_texturePointerIds.find(symbol.index) == std::end(m_texturePointerIds));
		auto pointerId = AllocateId();
		m_texturePointerIds[symbol.index] = pointerId;
	}
}

void CSpirvShaderGenerator::DecorateTextureIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEXTURE) continue;
		assert(m_texturePointerIds.find(symbol.index) != std::end(m_texturePointerIds));
		auto pointerId = m_texturePointerIds[symbol.index];
		WriteOp(spv::OpDecorate, pointerId, spv::DecorationDescriptorSet, 0);
		WriteOp(spv::OpDecorate, pointerId, spv::DecorationBinding, symbol.index);
	}
}

void CSpirvShaderGenerator::DeclareTextureIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEXTURE) continue;
		assert(m_texturePointerIds.find(symbol.index) != std::end(m_texturePointerIds));
		auto pointerId = m_texturePointerIds[symbol.index];
		WriteOp(spv::OpVariable, m_imageSamplerPointerTypeId, pointerId, spv::StorageClassUniformConstant);
	}
}

uint32 CSpirvShaderGenerator::LoadFromSymbol(const CShaderBuilder::SYMBOLREF& srcRef)
{
	uint32 srcId = 0;
	switch(srcRef.symbol.location)
	{
	case CShaderBuilder::SYMBOL_LOCATION_INPUT:
		{
			assert(srcRef.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4);
			srcId = AllocateId();
			assert(m_inputPointerIds.find(srcRef.symbol.index) != std::end(m_inputPointerIds));
			auto pointerId = m_inputPointerIds[srcRef.symbol.index];
			WriteOp(spv::OpLoad, m_float4TypeId, srcId, pointerId);
		}
		break;
	case CShaderBuilder::SYMBOL_LOCATION_UNIFORM:
		{
			assert(m_hasUniforms);
			srcId = AllocateId();
			auto memberPointerId = AllocateId();
			auto memberIndex = m_uniformStructMemberIndices[srcRef.symbol.index];
			assert(m_intConstantIds.find(memberIndex) != m_intConstantIds.end());
			auto intConstantId = m_intConstantIds[memberIndex];
			switch(srcRef.symbol.type)
			{
			case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
				WriteOp(spv::OpAccessChain, m_pushFloat4PointerTypeId, memberPointerId, m_pushUniformVariableId, intConstantId);
				WriteOp(spv::OpLoad, m_float4TypeId, srcId, memberPointerId);
				break;
			case CShaderBuilder::SYMBOL_TYPE_MATRIX:
				WriteOp(spv::OpAccessChain, m_pushMatrix44PointerTypeId, memberPointerId, m_pushUniformVariableId, intConstantId);
				WriteOp(spv::OpLoad, m_matrix44TypeId, srcId, memberPointerId);
				break;
			default:
				assert(false);
				break;
			}
		}
		break;
	case CShaderBuilder::SYMBOL_LOCATION_TEXTURE:
		{
			assert(m_hasTextures);
			assert(srcRef.symbol.type == CShaderBuilder::SYMBOL_TYPE_TEXTURE2D);
			srcId = AllocateId();
			assert(m_texturePointerIds.find(srcRef.symbol.index) != std::end(m_texturePointerIds));
			auto pointerId = m_texturePointerIds[srcRef.symbol.index];
			WriteOp(spv::OpLoad, m_imageSamplerTypeId, srcId, pointerId);
		}
		break;
	case CShaderBuilder::SYMBOL_LOCATION_TEMPORARY:
		{
			assert(m_temporaryValueIds.find(srcRef.symbol.index) != std::end(m_temporaryValueIds));
			srcId = m_temporaryValueIds[srcRef.symbol.index];
		}
		break;
	default:
		assert(false);
		break;
	}
	if(
		(srcRef.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4) &&
		!IsIdentitySwizzle(srcRef.swizzle)
	)
	{
		auto prevId = srcId;
		uint32 components[4] = { 0, 0, 0, 0 };
		srcId = AllocateId();
		switch(srcRef.swizzle)
		{
		case SWIZZLE_Z:
			components[0] = 2;
			break;
		case SWIZZLE_W:
			components[0] = 3;
			break;
		default:
			assert(false);
			break;
		}
		WriteOp(spv::OpVectorShuffle, m_float4TypeId, srcId, prevId, prevId, 
			components[0], components[1], components[2], components[3]);
	}
	return srcId;
}

void CSpirvShaderGenerator::StoreToSymbol(const CShaderBuilder::SYMBOLREF& dstRef, uint32 dstId)
{
	assert(IsIdentitySwizzle(dstRef.swizzle));
	assert(dstRef.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4);
	switch(dstRef.symbol.location)
	{
	case CShaderBuilder::SYMBOL_LOCATION_OUTPUT:
		{
			auto outputSemantic = m_shaderBuilder.GetOutputSemantic(dstRef.symbol);
			switch(outputSemantic.type)
			{
			case Nuanceur::SEMANTIC_SYSTEM_POSITION:
				{
					auto outputPositionPointerId = AllocateId();
					assert(m_intConstantIds.find(0) != m_intConstantIds.end());
					auto intConstantId = m_intConstantIds[0];
					WriteOp(spv::OpAccessChain, m_outputFloat4PointerTypeId, outputPositionPointerId, m_outputPerVertexVariableId, intConstantId);
					WriteOp(spv::OpStore, outputPositionPointerId, dstId);
				}
				break;
			case Nuanceur::SEMANTIC_TEXCOORD:
			case Nuanceur::SEMANTIC_SYSTEM_COLOR:
				{
					assert(m_outputPointerIds.find(dstRef.symbol.index) != std::end(m_outputPointerIds));
					auto pointerId = m_outputPointerIds[dstRef.symbol.index];
					WriteOp(spv::OpStore, pointerId, dstId);
				}
				break;
			default:
				assert(false);
				break;
			}
		}
		break;
	case CShaderBuilder::SYMBOL_LOCATION_TEMPORARY:
		//Replace current active id for that temporary symbol.
		m_temporaryValueIds[dstRef.symbol.index] = dstId;
		break;
	default:
		assert(false);
		break;
	}
}

void CSpirvShaderGenerator::Write32(uint32 value)
{
	m_outputStream.Write32(value);
}

uint32 CSpirvShaderGenerator::MapSemanticToLocation(Nuanceur::SEMANTIC semantic, uint32 index)
{
	//Position     -> 0 (only one position attribute supported)
	//Tex Coord    -> 1 + index
	//System Color -> 0 (used by the system after fragment shading)
	switch(semantic)
	{
	case Nuanceur::SEMANTIC_POSITION:
		assert(index == 0);
		return 0;
		break;
	case Nuanceur::SEMANTIC_TEXCOORD:
		return 1 + index;
		break;
	case Nuanceur::SEMANTIC_SYSTEM_COLOR:
		return 0;
		break;
	default:
		assert(false);
		break;
	}
	return 0;
}

uint32 CSpirvShaderGenerator::AllocateId()
{
	return m_nextId++;
}
