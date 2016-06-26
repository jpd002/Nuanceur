#include "SpirvShaderGenerator.h"

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

void CSpirvShaderGenerator::Generate()
{
	//Some notes:
	//Mali drivers seem to require a struct to be used for output vertex params

	Write32(spv::MagicNumber);
	Write32(spv::Version);     //This needs to be 0x10000
	Write32(0);                //Generator
	Write32(0);                //Bound
	Write32(0);                //Instruction Schema

	auto voidTypeId = AllocateId();
	auto mainFunctionTypeId = AllocateId();
	auto floatTypeId = AllocateId();
	auto float4TypeId = AllocateId();
	auto int32TypeId = AllocateId();
	auto perVertexStructTypeId = AllocateId();
	m_inputFloat4PointerTypeId = AllocateId();
	m_outputFloat4PointerTypeId = AllocateId();
	auto outputPerVertexStructPointerTypeId = AllocateId();

	AllocateInputPointerIds();
	AllocateOutputPointerIds();

	auto outputPerVertexVariableId = AllocateId();
	auto int32ZeroConstantId = AllocateId();

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
			outputPointerIds.push_back(outputPerVertexVariableId);
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
	}

	DecorateInputPointerIds();
	DecorateOutputPointerIds();

	//Type declarations
	WriteOp(spv::OpTypeVoid, voidTypeId);
	WriteOp(spv::OpTypeFunction, mainFunctionTypeId, voidTypeId);
	WriteOp(spv::OpTypeFloat, floatTypeId, 32);
	WriteOp(spv::OpTypeVector, float4TypeId, floatTypeId, 4);
	WriteOp(spv::OpTypeInt, int32TypeId, 32, 1);
	WriteOp(spv::OpTypeStruct, perVertexStructTypeId, float4TypeId, floatTypeId);
	WriteOp(spv::OpTypePointer, m_inputFloat4PointerTypeId, spv::StorageClassInput, float4TypeId);
	WriteOp(spv::OpTypePointer, m_outputFloat4PointerTypeId, spv::StorageClassOutput, float4TypeId);
	if(m_shaderType == SHADER_TYPE_VERTEX)
	{
		WriteOp(spv::OpTypePointer, outputPerVertexStructPointerTypeId, spv::StorageClassOutput, perVertexStructTypeId);
	}

	DeclareInputPointerIds();
	DeclareOutputPointerIds();

	//Declare Output PerVertex (Position + PointSize) BuiltIn
	if(m_shaderType == SHADER_TYPE_VERTEX)
	{
		WriteOp(spv::OpVariable, outputPerVertexStructPointerTypeId, outputPerVertexVariableId, spv::StorageClassOutput);
	}

	//Declare Zero Index Constant
	WriteOp(spv::OpConstant, int32TypeId, int32ZeroConstantId, 0);

	//Write main function
	{
		WriteOp(spv::OpFunction, voidTypeId, mainFunctionId, spv::FunctionControlMaskNone, mainFunctionTypeId);
		WriteOp(spv::OpLabel, mainFunctionLabelId);

		for(const auto& statement : m_shaderBuilder.GetStatements())
		{
			const auto& dstRef = statement.dstRef;
			const auto& src1Ref = statement.src1Ref;
			switch(statement.op)
			{
			case CShaderBuilder::STATEMENT_OP_ASSIGN:
				{
					uint32 tempId = AllocateId();
					assert(src1Ref.swizzle == SWIZZLE_XYZW);
					if(src1Ref.symbol.location == CShaderBuilder::SYMBOL_LOCATION_INPUT)
					{
						assert(m_inputPointerIds.find(src1Ref.symbol.index) != std::end(m_inputPointerIds));
						auto pointerId = m_inputPointerIds[src1Ref.symbol.index];
						WriteOp(spv::OpLoad, float4TypeId, tempId, pointerId);
					}
					else
					{
						assert(false);
					}
					if(dstRef.symbol.location == CShaderBuilder::SYMBOL_LOCATION_OUTPUT)
					{
						auto outputSemantic = m_shaderBuilder.GetOutputSemantic(dstRef.symbol);
						switch(outputSemantic.type)
						{
						case Nuanceur::SEMANTIC_SYSTEM_POSITION:
							{
								auto outputPositionPointerId = AllocateId();
								WriteOp(spv::OpAccessChain, m_outputFloat4PointerTypeId, outputPositionPointerId, outputPerVertexVariableId, int32ZeroConstantId);
								WriteOp(spv::OpStore, outputPositionPointerId, tempId);
							}
							break;
						case Nuanceur::SEMANTIC_TEXCOORD:
						case Nuanceur::SEMANTIC_SYSTEM_COLOR:
							{
								assert(m_outputPointerIds.find(dstRef.symbol.index) != std::end(m_outputPointerIds));
								auto pointerId = m_outputPointerIds[dstRef.symbol.index];
								WriteOp(spv::OpStore, pointerId, tempId);
							}
							break;
						default:
							assert(false);
							break;
						}
					}
					else
					{
						assert(false);
					}
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
//		auto semantic = m_shaderBuilder.GetInputSemantic(symbol);
		assert(m_inputPointerIds.find(symbol.index) != std::end(m_inputPointerIds));
		auto pointerId = m_inputPointerIds[symbol.index];
		WriteOp(spv::OpDecorate, pointerId, spv::DecorationLocation, symbol.index);
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
		uint32 location = semantic.index;
		//TODO: Use "map semantic to location"
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

void CSpirvShaderGenerator::Write32(uint32 value)
{
	m_outputStream.Write32(value);
}

uint32 CSpirvShaderGenerator::MapSemanticToLocation(Nuanceur::SEMANTIC semantic, uint32 index)
{
	//Position = 0
	//Tex Coord = 1 + (index)
	return 0;
}

uint32 CSpirvShaderGenerator::AllocateId()
{
	return m_nextId++;
}
