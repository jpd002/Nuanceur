#include <cstring>
#include <array>
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

static CShaderBuilder::SYMBOL_TYPE GetCommonSymbolType(const CShaderBuilder::SYMBOLREF& op1, const CShaderBuilder::SYMBOLREF& op2)
{
	assert(op1.symbol.type == op2.symbol.type);
	return op1.symbol.type;
}

static CShaderBuilder::SYMBOL_TYPE GetCommonSymbolType(const CShaderBuilder::SYMBOLREF& op1, const CShaderBuilder::SYMBOLREF& op2, const CShaderBuilder::SYMBOLREF& op3)
{
	assert(op1.symbol.type == op2.symbol.type);
	assert(op2.symbol.type == op3.symbol.type);
	return op1.symbol.type;
}

void CSpirvShaderGenerator::Generate()
{
	//Some notes:
	//Mali drivers seem to require a struct to be used for output vertex params

	Write32(spv::MagicNumber);
	Write32(0x00010300); //SPIR-V 1.3 (compatible with Vulkan 1.1)
	Write32(0);          //Generator
	Write32(0);          //Bound
	Write32(0);          //Instruction Schema

	m_hasTextures = std::count_if(m_shaderBuilder.GetSymbols().begin(), m_shaderBuilder.GetSymbols().end(),
	                              [](const CShaderBuilder::SYMBOL& symbol) { return symbol.location == CShaderBuilder::SYMBOL_LOCATION_TEXTURE; }) != 0;

	bool hasInvocationInterlock = std::count_if(m_shaderBuilder.GetStatements().begin(), m_shaderBuilder.GetStatements().end(),
	                                            [](const CShaderBuilder::STATEMENT& statement) { return statement.op == CShaderBuilder::STATEMENT_OP_INVOCATION_INTERLOCK_BEGIN; }) != 0;

	m_has8BitInt = std::count_if(m_shaderBuilder.GetStatements().begin(), m_shaderBuilder.GetStatements().end(),
	                             [](const CShaderBuilder::STATEMENT& statement) { return statement.op == CShaderBuilder::STATEMENT_OP_STORE8; }) != 0;
	if(!m_has8BitInt)
	{
		m_has8BitInt = std::count_if(m_shaderBuilder.GetSymbols().begin(), m_shaderBuilder.GetSymbols().end(),
		                             [](const CShaderBuilder::SYMBOL& symbol) { return symbol.type == CShaderBuilder::SYMBOL_TYPE_ARRAYUCHAR; }) != 0;
	}

	m_has16BitInt = std::count_if(m_shaderBuilder.GetStatements().begin(), m_shaderBuilder.GetStatements().end(),
	                              [](const CShaderBuilder::STATEMENT& statement) { return statement.op == CShaderBuilder::STATEMENT_OP_STORE16; }) != 0;
	if(!m_has16BitInt)
	{
		m_has16BitInt = std::count_if(m_shaderBuilder.GetSymbols().begin(), m_shaderBuilder.GetSymbols().end(),
		                              [](const CShaderBuilder::SYMBOL& symbol) { return symbol.type == CShaderBuilder::SYMBOL_TYPE_ARRAYUSHORT; }) != 0;
	}
	// 16bit writes requires 8 bits buffer
	m_has8BitInt |= m_has16BitInt;

	auto voidTypeId = AllocateId();
	auto mainFunctionTypeId = AllocateId();
	m_glslStd450ExtInst = AllocateId();
	m_boolTypeId = AllocateId();
	m_bool4TypeId = AllocateId();
	m_floatTypeId = AllocateId();
	m_float4TypeId = AllocateId();

	m_boolConstantFalseId = AllocateId();
	m_boolConstantTrueId = AllocateId();

	m_uintTypeId = AllocateId();
	m_ushortTypeId = AllocateId();
	m_ucharTypeId = AllocateId();

	m_uchar4TypeId = AllocateId();
	m_ushort4TypeId = AllocateId();
	m_uint4TypeId = AllocateId();
	m_matrix44TypeId = AllocateId();

	m_uintArrayTypeId = AllocateId();
	m_ushortArrayTypeId = AllocateId();
	m_ucharArrayTypeId = AllocateId();

	m_intTypeId = AllocateId();
	m_shortTypeId = AllocateId();
	m_charTypeId = AllocateId();

	m_int2TypeId = AllocateId();
	m_int3TypeId = AllocateId();
	m_int4TypeId = AllocateId();
	auto perVertexStructTypeId = AllocateId();
	m_inputFloat4PointerTypeId = AllocateId();
	m_inputIntPointerTypeId = AllocateId();
	m_inputInt3PointerTypeId = AllocateId();
	m_inputUint4PointerTypeId = AllocateId();
	m_outputFloatPointerTypeId = AllocateId();
	m_outputFloat4PointerTypeId = AllocateId();
	m_outputUint4PointerTypeId = AllocateId();
	m_functionFloat4PointerTypeId = AllocateId();
	m_functionInt4PointerTypeId = AllocateId();
	m_functionUint4PointerTypeId = AllocateId();
	m_functionBool4PointerTypeId = AllocateId();
	auto outputPerVertexStructPointerTypeId = AllocateId();

	AllocateUniformStructsIds();
	if(!m_structInfos.empty())
	{
		m_pushFloat4PointerTypeId = AllocateId();
		m_pushInt4PointerTypeId = AllocateId();
		m_pushMatrix44PointerTypeId = AllocateId();

		m_uniformFloat4PointerTypeId = AllocateId();
		m_uniformInt4PointerTypeId = AllocateId();
		m_uniformUintPtrId = AllocateId();
		m_uniformUint16PtrId = AllocateId();
		m_uniformUint8PtrId = AllocateId();
	}

	if(m_hasTextures)
	{
		//Sampled image
		m_sampledImage2DTypeId = AllocateId();
		m_sampledImageSamplerTypeId = AllocateId();
		m_sampledImageSamplerPointerTypeId = AllocateId();

		//Storage image
		m_storageImage2DTypeId = AllocateId();
		m_storageImage2DPointerTypeId = AllocateId();
		m_imageUintPtrId = AllocateId();

		//Subpass Input
		m_subpassInputTypeId = AllocateId();
		m_subpassInputUintTypeId = AllocateId();
		m_subpassInputPointerTypeId = AllocateId();
		m_subpassInputUintPointerTypeId = AllocateId();

		AllocateTextureIds();
	}

	AllocateInputPointerIds();
	AllocateOutputPointerIds();
	AllocateVariablePointerIds();

	m_outputPerVertexVariableId = AllocateId();

	auto mainFunctionId = AllocateId();
	auto mainFunctionLabelId = AllocateId();

	WriteOp(spv::OpCapability, spv::CapabilityShader);
	WriteOp(spv::OpCapability, spv::CapabilityInputAttachment);
	if(m_has8BitInt)
	{
		WriteOp(spv::OpCapability, spv::CapabilityInt8);
		WriteOp(spv::OpCapability, spv::CapabilityStorageBuffer8BitAccess);
		WriteOp(spv::OpCapability, spv::CapabilityUniformAndStorageBuffer8BitAccess);
	}

	if(m_has16BitInt)
	{
		WriteOp(spv::OpCapability, spv::CapabilityInt16);
		WriteOp(spv::OpCapability, spv::CapabilityStorageBuffer16BitAccess);
		WriteOp(spv::OpCapability, spv::CapabilityUniformAndStorageBuffer16BitAccess);
	}

	if(hasInvocationInterlock)
	{
		WriteOp(spv::OpCapability, spv::CapabilityFragmentShaderPixelInterlockEXT);
		WriteOp(spv::OpExtension, "SPV_EXT_fragment_shader_interlock");
	}
	if(m_has8BitInt || m_has16BitInt)
	{
		// note: is this needed?
		WriteOp(spv::OpExtension, "SPV_KHR_storage_buffer_storage_class");
		if(m_has8BitInt)
			WriteOp(spv::OpExtension, "SPV_KHR_8bit_storage");
		if(m_has16BitInt)
			WriteOp(spv::OpExtension, "SPV_KHR_16bit_storage");
	}
	WriteOp(spv::OpExtInstImport, m_glslStd450ExtInst, "GLSL.std.450");
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
		case SHADER_TYPE_COMPUTE:
			executionModel = spv::ExecutionModelGLCompute;
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
		        inputPointerIds, outputPointerIds);
	}

	if(m_shaderType == SHADER_TYPE_FRAGMENT)
	{
		WriteOp(spv::OpExecutionMode, mainFunctionId, spv::ExecutionModeOriginUpperLeft);
		if(hasInvocationInterlock)
		{
			WriteOp(spv::OpExecutionMode, mainFunctionId, spv::ExecutionModePixelInterlockOrderedEXT);
		}
	}
	else if(m_shaderType == SHADER_TYPE_COMPUTE)
	{
		uint32 localSizeX = m_shaderBuilder.GetMetadata(CShaderBuilder::METADATA_LOCALSIZE_X, 1);
		uint32 localSizeY = m_shaderBuilder.GetMetadata(CShaderBuilder::METADATA_LOCALSIZE_Y, 1);
		uint32 localSizeZ = m_shaderBuilder.GetMetadata(CShaderBuilder::METADATA_LOCALSIZE_Z, 1);
		WriteOp(spv::OpExecutionMode, mainFunctionId, spv::ExecutionModeLocalSize, localSizeX, localSizeY, localSizeZ);
	}

	//Names
	//WriteOp(spv::OpSource, spv::SourceLanguageUnknown, 100);
	//WriteOp(spv::OpName, mainFunctionId, "main");

	WriteVariablePointerNames();
	WriteUniformStructNames();

	//Annotations
	if(m_shaderType == SHADER_TYPE_VERTEX)
	{
		WriteOp(spv::OpMemberDecorate, perVertexStructTypeId, VERTEX_OUTPUT_POSITION_INDEX, spv::DecorationBuiltIn, spv::BuiltInPosition);
		WriteOp(spv::OpMemberDecorate, perVertexStructTypeId, VERTEX_OUTPUT_POINTSIZE_INDEX, spv::DecorationBuiltIn, spv::BuiltInPointSize);
		WriteOp(spv::OpDecorate, perVertexStructTypeId, spv::DecorationBlock);
		RegisterIntConstant(VERTEX_OUTPUT_POSITION_INDEX); //Will be required when accessing built-in position
		RegisterIntConstant(VERTEX_OUTPUT_POINTSIZE_INDEX);
	}

	DecorateUniformStructIds();

	if(m_hasTextures)
	{
		DecorateTextureIds();
	}

	DecorateInputPointerIds();
	DecorateOutputPointerIds();

	WriteOp(spv::OpDecorate, m_uintArrayTypeId, spv::DecorationArrayStride, 4); //Make this optional
	if(m_has8BitInt)
		WriteOp(spv::OpDecorate, m_ucharArrayTypeId, spv::DecorationArrayStride, 1);
	if(m_has16BitInt)
		WriteOp(spv::OpDecorate, m_ushortArrayTypeId, spv::DecorationArrayStride, 2);

	//Type declarations
	WriteOp(spv::OpTypeVoid, voidTypeId);
	WriteOp(spv::OpTypeFunction, mainFunctionTypeId, voidTypeId);
	WriteOp(spv::OpTypeBool, m_boolTypeId);
	WriteOp(spv::OpTypeVector, m_bool4TypeId, m_boolTypeId, 4);
	WriteOp(spv::OpTypeFloat, m_floatTypeId, 32);
	WriteOp(spv::OpTypeVector, m_float4TypeId, m_floatTypeId, 4);
	WriteOp(spv::OpTypeMatrix, m_matrix44TypeId, m_float4TypeId, 4);
	WriteOp(spv::OpTypeInt, m_intTypeId, 32, 1);
	WriteOp(spv::OpTypeVector, m_int2TypeId, m_intTypeId, 2);
	WriteOp(spv::OpTypeVector, m_int4TypeId, m_intTypeId, 4);
	WriteOp(spv::OpTypeInt, m_uintTypeId, 32, 0);
	if(m_has8BitInt)
	{
		WriteOp(spv::OpTypeInt, m_charTypeId, 8, 1);
		WriteOp(spv::OpTypeInt, m_ucharTypeId, 8, 0);
		WriteOp(spv::OpTypeRuntimeArray, m_ucharArrayTypeId, m_ucharTypeId);
		WriteOp(spv::OpTypeVector, m_uchar4TypeId, m_ucharTypeId, 4);
	}
	if(m_has16BitInt)
	{
		WriteOp(spv::OpTypeInt, m_shortTypeId, 16, 1);
		WriteOp(spv::OpTypeInt, m_ushortTypeId, 16, 0);
		WriteOp(spv::OpTypeRuntimeArray, m_ushortArrayTypeId, m_ushortTypeId);
		WriteOp(spv::OpTypeVector, m_ushort4TypeId, m_ushortTypeId, 4);
	}
	WriteOp(spv::OpTypeVector, m_uint4TypeId, m_uintTypeId, 4);
	WriteOp(spv::OpTypeRuntimeArray, m_uintArrayTypeId, m_uintTypeId); //Make this optional
	WriteOp(spv::OpTypePointer, m_inputFloat4PointerTypeId, spv::StorageClassInput, m_float4TypeId);
	WriteOp(spv::OpTypePointer, m_inputUint4PointerTypeId, spv::StorageClassInput, m_uint4TypeId);
	WriteOp(spv::OpTypePointer, m_outputFloatPointerTypeId, spv::StorageClassOutput, m_floatTypeId);
	WriteOp(spv::OpTypePointer, m_outputFloat4PointerTypeId, spv::StorageClassOutput, m_float4TypeId);
	WriteOp(spv::OpTypePointer, m_outputUint4PointerTypeId, spv::StorageClassOutput, m_uint4TypeId);
	WriteOp(spv::OpTypePointer, m_functionFloat4PointerTypeId, spv::StorageClassFunction, m_float4TypeId);
	WriteOp(spv::OpTypePointer, m_functionInt4PointerTypeId, spv::StorageClassFunction, m_int4TypeId);
	WriteOp(spv::OpTypePointer, m_functionUint4PointerTypeId, spv::StorageClassFunction, m_uint4TypeId);
	WriteOp(spv::OpTypePointer, m_functionBool4PointerTypeId, spv::StorageClassFunction, m_bool4TypeId);

	if(m_shaderType == SHADER_TYPE_VERTEX)
	{
		WriteOp(spv::OpTypeStruct, perVertexStructTypeId, m_float4TypeId, m_floatTypeId);
		WriteOp(spv::OpTypePointer, outputPerVertexStructPointerTypeId, spv::StorageClassOutput, perVertexStructTypeId);
		WriteOp(spv::OpTypePointer, m_inputIntPointerTypeId, spv::StorageClassInput, m_intTypeId);
	}
	else if(m_shaderType == SHADER_TYPE_COMPUTE)
	{
		WriteOp(spv::OpTypeVector, m_int3TypeId, m_intTypeId, 3);
		WriteOp(spv::OpTypePointer, m_inputInt3PointerTypeId, spv::StorageClassInput, m_int3TypeId);
	}

	DeclareUniformStructIds();

	if(m_hasTextures)
	{
		//Sampled image
		WriteOp(spv::OpTypeImage, m_sampledImage2DTypeId, m_floatTypeId, spv::Dim2D, 0, 0, 0, 1, spv::ImageFormatUnknown);
		WriteOp(spv::OpTypeSampledImage, m_sampledImageSamplerTypeId, m_sampledImage2DTypeId);
		WriteOp(spv::OpTypePointer, m_sampledImageSamplerPointerTypeId, spv::StorageClassUniformConstant, m_sampledImageSamplerTypeId);

		//Storage image
		WriteOp(spv::OpTypeImage, m_storageImage2DTypeId, m_uintTypeId, spv::Dim2D, 0, 0, 0, 2, spv::ImageFormatR32ui);
		WriteOp(spv::OpTypePointer, m_storageImage2DPointerTypeId, spv::StorageClassUniformConstant, m_storageImage2DTypeId);
		WriteOp(spv::OpTypePointer, m_imageUintPtrId, spv::StorageClassImage, m_uintTypeId);

		//Subpass input
		WriteOp(spv::OpTypeImage, m_subpassInputTypeId, m_floatTypeId, spv::DimSubpassData, 0, 0, 0, 2, spv::ImageFormatUnknown);
		WriteOp(spv::OpTypePointer, m_subpassInputPointerTypeId, spv::StorageClassUniformConstant, m_subpassInputTypeId);

		WriteOp(spv::OpTypeImage, m_subpassInputUintTypeId, m_uintTypeId, spv::DimSubpassData, 0, 0, 0, 2, spv::ImageFormatUnknown);
		WriteOp(spv::OpTypePointer, m_subpassInputUintPointerTypeId, spv::StorageClassUniformConstant, m_subpassInputUintTypeId);
	}

	DeclareInputPointerIds();
	DeclareOutputPointerIds();

	//Declare Output PerVertex (Position + PointSize) BuiltIn
	if(m_shaderType == SHADER_TYPE_VERTEX)
	{
		WriteOp(spv::OpVariable, outputPerVertexStructPointerTypeId, m_outputPerVertexVariableId, spv::StorageClassOutput);
	}

	for(auto& structInfoPair : m_structInfos)
	{
		auto& structInfo = structInfoPair.second;
		auto structUnit = structInfoPair.first;
		if(structUnit == Nuanceur::UNIFORM_UNIT_PUSHCONSTANT)
		{
			WriteOp(spv::OpVariable, structInfo.pointerTypeId, structInfo.variableId, spv::StorageClassPushConstant);
		}
		else
		{
			WriteOp(spv::OpVariable, structInfo.pointerTypeId, structInfo.variableId, spv::StorageClassUniform);
		}
	}

	if(m_hasTextures)
	{
		DeclareTextureIds();
	}

	GatherConstantsFromTemps();

	//Declare Float Constants
	for(const auto& floatConstantIdPair : m_floatConstantIds)
	{
		assert(m_floatTypeId != EMPTY_ID);
		WriteOp(spv::OpConstant, m_floatTypeId, floatConstantIdPair.second, floatConstantIdPair.first);
	}

	//Declare Int Constants
	for(const auto& intConstantIdPair : m_intConstantIds)
	{
		assert(m_intTypeId != EMPTY_ID);
		WriteOp(spv::OpConstant, m_intTypeId, intConstantIdPair.second, intConstantIdPair.first);
	}

	//Declare Uint Constants
	for(const auto& uintConstantIdPair : m_uintConstantIds)
	{
		assert(m_uintTypeId != EMPTY_ID);
		WriteOp(spv::OpConstant, m_uintTypeId, uintConstantIdPair.second, uintConstantIdPair.first);
	}

	//Declare Ushort Constants
	for(const auto& ushortConstantIdPair : m_ushortConstantIds)
	{
		assert(m_ushortTypeId != EMPTY_ID);
		WriteOp(spv::OpConstant, m_ushortTypeId, ushortConstantIdPair.second, ushortConstantIdPair.first);
	}

	//Declare Uchar Constants
	for(const auto& ucharConstantIdPair : m_ucharConstantIds)
	{
		assert(m_ucharTypeId != EMPTY_ID);
		WriteOp(spv::OpConstant, m_ucharTypeId, ucharConstantIdPair.second, ucharConstantIdPair.first);
	}

	//Declare Bool Constants
	WriteOp(spv::OpConstantFalse, m_boolTypeId, m_boolConstantFalseId);
	WriteOp(spv::OpConstantTrue, m_boolTypeId, m_boolConstantTrueId);

	DeclareTemporaryValueIds();

	bool returnInBlock = false;

	//Write main function
	{
		WriteOp(spv::OpFunction, voidTypeId, mainFunctionId, spv::FunctionControlMaskNone, mainFunctionTypeId);
		WriteOp(spv::OpLabel, mainFunctionLabelId);

		DeclareVariablePointerIds();

		for(const auto& statement : m_shaderBuilder.GetStatements())
		{
			const auto& dstRef = statement.dstRef;
			const auto& src1Ref = statement.src1Ref;
			const auto& src2Ref = statement.src2Ref;
			const auto& src3Ref = statement.src3Ref;
			const auto& src4Ref = statement.src4Ref;
			switch(statement.op)
			{
			case CShaderBuilder::STATEMENT_OP_ADD:
				Add(dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_SUBSTRACT:
				Sub(dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_MULTIPLY:
			{
				auto src1Id = LoadFromSymbol(src1Ref);
				auto src2Id = LoadFromSymbol(src2Ref);
				auto resultId = AllocateId();
				if(
				    (src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX) &&
				    (src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4))
				{
					WriteOp(spv::OpMatrixTimesVector, m_float4TypeId, resultId, src1Id, src2Id);
				}
				else if(
				    (src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4) &&
				    (src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4))
				{
					WriteOp(spv::OpFMul, m_float4TypeId, resultId, src1Id, src2Id);
				}
				else if(
				    (src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_INT4) &&
				    (src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_INT4))
				{
					WriteOp(spv::OpIMul, m_int4TypeId, resultId, src1Id, src2Id);
				}
				else if(
				    (src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_UINT4) &&
				    (src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_UINT4))
				{
					WriteOp(spv::OpIMul, m_uint4TypeId, resultId, src1Id, src2Id);
				}
				else
				{
					assert(false);
				}
				StoreToSymbol(dstRef, resultId);
			}
			break;
			case CShaderBuilder::STATEMENT_OP_DIVIDE:
				Div(dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_MODULO:
				Mod(dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_ABS:
				GlslStdOp(GLSLstd450FAbs, dstRef, src1Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_CLAMP:
				Clamp(dstRef, src1Ref, src2Ref, src3Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_FRACT:
				GlslStdOp(GLSLstd450Fract, dstRef, src1Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_TRUNC:
				GlslStdOp(GLSLstd450Trunc, dstRef, src1Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_ISINF:
				ClassifyFloat(spv::OpIsInf, dstRef, src1Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_LOG2:
				GlslStdOp(GLSLstd450Log2, dstRef, src1Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_MIN:
				Min(dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_MAX:
				Max(dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_DOT:
				Dot(dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_MIX:
				Mix(dstRef, src1Ref, src2Ref, src3Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_AND:
				BitwiseOp(spv::OpBitwiseAnd, dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_OR:
				BitwiseOp(spv::OpBitwiseOr, dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_XOR:
				BitwiseOp(spv::OpBitwiseXor, dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_NOT:
				BitwiseNot(dstRef, src1Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_LSHIFT:
				BitwiseOp(spv::OpShiftLeftLogical, dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_RSHIFT:
				BitwiseOp(spv::OpShiftRightLogical, dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_RSHIFT_ARITHMETIC:
				BitwiseOp(spv::OpShiftRightArithmetic, dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_LOGICAL_AND:
				LogicalOp(spv::OpLogicalAnd, dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_LOGICAL_OR:
				LogicalOp(spv::OpLogicalOr, dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_LOGICAL_NOT:
				LogicalNot(dstRef, src1Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_COMPARE_EQ:
			case CShaderBuilder::STATEMENT_OP_COMPARE_NE:
			case CShaderBuilder::STATEMENT_OP_COMPARE_LT:
			case CShaderBuilder::STATEMENT_OP_COMPARE_LE:
			case CShaderBuilder::STATEMENT_OP_COMPARE_GT:
			case CShaderBuilder::STATEMENT_OP_COMPARE_GE:
				Compare(statement.op, dstRef, src1Ref, src2Ref);
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
			case CShaderBuilder::STATEMENT_OP_LOAD:
				Load(dstRef, src1Ref, src2Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_STORE:
				Store(src1Ref, src2Ref, src3Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_STORE16:
				Store16(src1Ref, src2Ref, src3Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_STORE8:
				Store8(src1Ref, src2Ref, src3Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_ATOMICAND:
				AtomicImageOp(spv::OpAtomicAnd, dstRef, src1Ref, src2Ref, src3Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_ATOMICOR:
				AtomicImageOp(spv::OpAtomicOr, dstRef, src1Ref, src2Ref, src3Ref);
				break;
			case CShaderBuilder::STATEMENT_OP_TOFLOAT:
			{
				auto src1Id = LoadFromSymbol(src1Ref);
				auto resultId = AllocateId();
				switch(src1Ref.symbol.type)
				{
				case CShaderBuilder::SYMBOL_TYPE_INT4:
					WriteOp(spv::OpConvertSToF, m_float4TypeId, resultId, src1Id);
					break;
				case CShaderBuilder::SYMBOL_TYPE_UINT4:
					WriteOp(spv::OpConvertUToF, m_float4TypeId, resultId, src1Id);
					break;
				default:
					assert(false);
					break;
				}
				StoreToSymbol(dstRef, resultId);
			}
			break;
			case CShaderBuilder::STATEMENT_OP_TOINT:
			{
				auto src1Id = LoadFromSymbol(src1Ref);
				auto resultId = AllocateId();
				switch(src1Ref.symbol.type)
				{
				case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
					WriteOp(spv::OpConvertFToS, m_int4TypeId, resultId, src1Id);
					break;
				case CShaderBuilder::SYMBOL_TYPE_UINT4:
					WriteOp(spv::OpBitcast, m_int4TypeId, resultId, src1Id);
					break;
				default:
					assert(false);
					break;
				}
				StoreToSymbol(dstRef, resultId);
			}
			break;
			case CShaderBuilder::STATEMENT_OP_TOUINT:
			{
				auto src1Id = LoadFromSymbol(src1Ref);
				auto resultId = AllocateId();
				switch(src1Ref.symbol.type)
				{
				case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
					WriteOp(spv::OpConvertFToU, m_uint4TypeId, resultId, src1Id);
					break;
				case CShaderBuilder::SYMBOL_TYPE_INT4:
					WriteOp(spv::OpBitcast, m_uint4TypeId, resultId, src1Id);
					break;
				default:
					assert(false);
					break;
				}
				StoreToSymbol(dstRef, resultId);
			}
			break;
			case CShaderBuilder::STATEMENT_OP_TOUCHAR:
			{
				auto src1Id = LoadFromSymbol(src1Ref);
				auto resultId = AllocateId();
				switch(src1Ref.symbol.type)
				{
				case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
					WriteOp(spv::OpConvertFToU, m_uchar4TypeId, resultId, src1Id);
					break;
				case CShaderBuilder::SYMBOL_TYPE_UINT4:
					WriteOp(spv::OpUConvert, m_uchar4TypeId, resultId, src1Id);
					break;
				default:
					assert(false);
					break;
				}
				StoreToSymbol(dstRef, resultId);
			}
			break;
			case CShaderBuilder::STATEMENT_OP_TOUSHORT:
			{
				auto src1Id = LoadFromSymbol(src1Ref);
				auto resultId = AllocateId();
				switch(src1Ref.symbol.type)
				{
				case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
					WriteOp(spv::OpConvertFToU, m_ushort4TypeId, resultId, src1Id);
					break;
				case CShaderBuilder::SYMBOL_TYPE_UINT4:
					WriteOp(spv::OpUConvert, m_ushort4TypeId, resultId, src1Id);
					break;
				default:
					assert(false);
					break;
				}
				StoreToSymbol(dstRef, resultId);
			}
			break;
			case CShaderBuilder::STATEMENT_OP_NEWVECTOR2:
			{
				auto resultType = GetResultType(statement.dstRef.symbol.type);
				assert(statement.GetSourceCount() == 2);
				assert(GetSwizzleElementCount(statement.src1Ref.swizzle) == 1);
				assert(GetSwizzleElementCount(statement.src2Ref.swizzle) == 1);

				auto src1Id = LoadFromSymbol(src1Ref);
				auto src2Id = LoadFromSymbol(src2Ref);
				auto resultId = AllocateId();
				WriteOp(spv::OpVectorShuffle, resultType, resultId, src1Id, src2Id, 0, 4, 0, 0);
				StoreToSymbol(dstRef, resultId);
			}
			break;
			case CShaderBuilder::STATEMENT_OP_NEWVECTOR4:
			{
				auto resultType = GetResultType(statement.dstRef.symbol.type);
				switch(statement.GetSourceCount())
				{
				case 2:
				{
					uint32 src1ElementCount = GetSwizzleElementCount(statement.src1Ref.swizzle);
					uint32 src2ElementCount = GetSwizzleElementCount(statement.src2Ref.swizzle);
					assert((src1ElementCount + src2ElementCount) == 4);
					if(
					    (src1ElementCount == 3) &&
					    (src2ElementCount == 1))
					{
						auto src1Id = LoadFromSymbol(src1Ref);
						auto src2Id = LoadFromSymbol(src2Ref);
						auto resultId = AllocateId();
						WriteOp(spv::OpVectorShuffle, resultType, resultId, src1Id, src2Id, 0, 1, 2, 4);
						StoreToSymbol(dstRef, resultId);
					}
					else if(
					    (statement.src1Ref.swizzle == SWIZZLE_X) &&
					    (statement.src2Ref.swizzle == SWIZZLE_XYZ))
					{
						auto src1Id = LoadFromSymbol(src1Ref);
						auto src2Id = LoadFromSymbol(src2Ref);
						auto resultId = AllocateId();
						WriteOp(spv::OpVectorShuffle, resultType, resultId, src1Id, src2Id, 0, 4, 5, 6);
						StoreToSymbol(dstRef, resultId);
					}
					else if(
					    (statement.src1Ref.swizzle == SWIZZLE_XY) &&
					    (statement.src2Ref.swizzle == SWIZZLE_XY))
					{
						auto src1Id = LoadFromSymbol(src1Ref);
						auto src2Id = LoadFromSymbol(src2Ref);
						auto resultId = AllocateId();
						WriteOp(spv::OpVectorShuffle, resultType, resultId, src1Id, src2Id, 0, 1, 4, 5);
						StoreToSymbol(dstRef, resultId);
					}
					else
					{
						assert(false);
					}
				}
				break;
				case 4:
					if(
					    (statement.src1Ref.swizzle == SWIZZLE_X) &&
					    (statement.src2Ref.swizzle == SWIZZLE_X) &&
					    (statement.src3Ref.swizzle == SWIZZLE_X) &&
					    (statement.src4Ref.swizzle == SWIZZLE_X))
					{
						auto src1Id = LoadFromSymbol(src1Ref);
						auto src2Id = LoadFromSymbol(src2Ref);
						auto src3Id = LoadFromSymbol(src3Ref);
						auto src4Id = LoadFromSymbol(src4Ref);
						auto resultInterId1 = AllocateId();
						auto resultInterId2 = AllocateId();
						auto resultId = AllocateId();
						WriteOp(spv::OpVectorShuffle, resultType, resultInterId1, src1Id, src2Id, 0, 4, 0, 0);
						WriteOp(spv::OpVectorShuffle, resultType, resultInterId2, src3Id, src4Id, 0, 4, 0, 0);
						WriteOp(spv::OpVectorShuffle, resultType, resultId, resultInterId1, resultInterId2, 0, 1, 4, 5);
						StoreToSymbol(dstRef, resultId);
					}
					else
					{
						assert(false);
					}
					break;
				default:
					assert(false);
					break;
				}
			}
			break;
			case CShaderBuilder::STATEMENT_OP_ASSIGN:
			{
				auto src1Id = LoadFromSymbol(src1Ref);
				StoreToSymbol(dstRef, src1Id);
			}
			break;
			case CShaderBuilder::STATEMENT_OP_RETURN:
				assert(!returnInBlock);
				returnInBlock = true;
				WriteOp(spv::OpReturn);
				break;
			case CShaderBuilder::STATEMENT_OP_INVOCATION_INTERLOCK_BEGIN:
				WriteOp(spv::OpBeginInvocationInterlockEXT);
				break;
			case CShaderBuilder::STATEMENT_OP_INVOCATION_INTERLOCK_END:
				WriteOp(spv::OpEndInvocationInterlockEXT);
				break;
			case CShaderBuilder::STATEMENT_OP_IF_BEGIN:
			{
				assert(src1Ref.swizzle == SWIZZLE_X);
				auto src1Id = LoadFromSymbol(src1Ref);
				auto beginLabelId = AllocateId();
				auto endLabelId = AllocateId();
				auto conditionId = AllocateId();
				WriteOp(spv::OpCompositeExtract, m_boolTypeId, conditionId, src1Id, 0);
				WriteOp(spv::OpSelectionMerge, endLabelId, spv::SelectionControlMaskNone);
				WriteOp(spv::OpBranchConditional, conditionId, beginLabelId, endLabelId);
				WriteOp(spv::OpLabel, beginLabelId);
				m_endLabelIds.push(endLabelId);
			}
			break;
			case CShaderBuilder::STATEMENT_OP_IF_END:
			{
				assert(m_endLabelIds.size() > 0);
				auto endLabelId = m_endLabelIds.top();
				if(!returnInBlock)
				{
					WriteOp(spv::OpBranch, endLabelId);
				}
				WriteOp(spv::OpLabel, endLabelId);
				m_endLabelIds.pop();
				returnInBlock = false;
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
		auto pointerId = m_inputPointerIds[symbol.index];
		switch(semantic.type)
		{
		case Nuanceur::SEMANTIC_SYSTEM_POSITION:
			assert(m_shaderType == SHADER_TYPE_FRAGMENT);
			WriteOp(spv::OpDecorate, pointerId, spv::DecorationBuiltIn, spv::BuiltInFragCoord);
			break;
		case Nuanceur::SEMANTIC_SYSTEM_VERTEXINDEX:
			assert(m_shaderType == SHADER_TYPE_VERTEX);
			WriteOp(spv::OpDecorate, pointerId, spv::DecorationBuiltIn, spv::BuiltInVertexIndex);
			break;
		case Nuanceur::SEMANTIC_SYSTEM_GIID:
			assert(m_shaderType == SHADER_TYPE_COMPUTE);
			WriteOp(spv::OpDecorate, pointerId, spv::DecorationBuiltIn, spv::BuiltInGlobalInvocationId);
			break;
		default:
		{
			auto location = MapSemanticToLocation(semantic.type, semantic.index);
			WriteOp(spv::OpDecorate, pointerId, spv::DecorationLocation, location);
		}
		break;
		}
	}
}

void CSpirvShaderGenerator::DeclareInputPointerIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_INPUT) continue;
		auto semantic = m_shaderBuilder.GetInputSemantic(symbol);
		assert(m_inputPointerIds.find(symbol.index) != std::end(m_inputPointerIds));
		auto pointerId = m_inputPointerIds[symbol.index];
		switch(semantic.type)
		{
		case Nuanceur::SEMANTIC_SYSTEM_VERTEXINDEX:
			assert(symbol.type == CShaderBuilder::SYMBOL_TYPE_INT4);
			WriteOp(spv::OpVariable, m_inputIntPointerTypeId, pointerId, spv::StorageClassInput);
			break;
		case Nuanceur::SEMANTIC_SYSTEM_GIID:
			assert(symbol.type == CShaderBuilder::SYMBOL_TYPE_INT4);
			WriteOp(spv::OpVariable, m_inputInt3PointerTypeId, pointerId, spv::StorageClassInput);
			break;
		default:
			switch(symbol.type)
			{
			case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
				WriteOp(spv::OpVariable, m_inputFloat4PointerTypeId, pointerId, spv::StorageClassInput);
				break;
			case CShaderBuilder::SYMBOL_TYPE_UINT4:
				WriteOp(spv::OpVariable, m_inputUint4PointerTypeId, pointerId, spv::StorageClassInput);
				break;
			default:
				assert(false);
				break;
			}
			break;
		}
	}
}

void CSpirvShaderGenerator::AllocateOutputPointerIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_OUTPUT) continue;
		auto semantic = m_shaderBuilder.GetOutputSemantic(symbol);
		if(IsBuiltInOutput(semantic.type)) continue;
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
		if(IsBuiltInOutput(semantic.type)) continue;
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
		if(IsBuiltInOutput(semantic.type)) continue;
		assert(m_outputPointerIds.find(symbol.index) != std::end(m_outputPointerIds));
		auto pointerId = m_outputPointerIds[symbol.index];
		switch(symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			WriteOp(spv::OpVariable, m_outputFloat4PointerTypeId, pointerId, spv::StorageClassOutput);
			break;
		case CShaderBuilder::SYMBOL_TYPE_UINT4:
			WriteOp(spv::OpVariable, m_outputUint4PointerTypeId, pointerId, spv::StorageClassOutput);
			break;
		default:
			assert(false);
			break;
		}
	}
}

uint32 CSpirvShaderGenerator::GetOutputPointerId(const CShaderBuilder::SYMBOLREF& symbol)
{
	assert(symbol.symbol.location == CShaderBuilder::SYMBOL_LOCATION_OUTPUT);
	auto outputSemantic = m_shaderBuilder.GetOutputSemantic(symbol.symbol);
	uint32 pointerId = 0;
	switch(outputSemantic.type)
	{
	case Nuanceur::SEMANTIC_SYSTEM_POSITION:
	{
		assert(m_shaderType == SHADER_TYPE_VERTEX);
		pointerId = AllocateId();
		assert(m_intConstantIds.find(VERTEX_OUTPUT_POSITION_INDEX) != m_intConstantIds.end());
		auto intConstantId = m_intConstantIds[VERTEX_OUTPUT_POSITION_INDEX];
		WriteOp(spv::OpAccessChain, m_outputFloat4PointerTypeId, pointerId, m_outputPerVertexVariableId, intConstantId);
	}
	break;
	case Nuanceur::SEMANTIC_SYSTEM_POINTSIZE:
	{
		assert(m_shaderType == SHADER_TYPE_VERTEX);
		pointerId = AllocateId();
		assert(m_intConstantIds.find(VERTEX_OUTPUT_POINTSIZE_INDEX) != m_intConstantIds.end());
		auto intConstantId = m_intConstantIds[VERTEX_OUTPUT_POINTSIZE_INDEX];
		WriteOp(spv::OpAccessChain, m_outputFloatPointerTypeId, pointerId, m_outputPerVertexVariableId, intConstantId);
	}
	break;
	case Nuanceur::SEMANTIC_TEXCOORD:
	case Nuanceur::SEMANTIC_SYSTEM_COLOR:
	{
		assert(m_outputPointerIds.find(symbol.symbol.index) != std::end(m_outputPointerIds));
		pointerId = m_outputPointerIds[symbol.symbol.index];
	}
	break;
	default:
		assert(false);
		break;
	}
	return pointerId;
}

void CSpirvShaderGenerator::GatherConstantsFromTemps()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEMPORARY) continue;
		switch(symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValue(symbol);
			RegisterFloatConstant(temporaryValue.x);
			RegisterFloatConstant(temporaryValue.y);
			RegisterFloatConstant(temporaryValue.z);
			RegisterFloatConstant(temporaryValue.w);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_INT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueInt(symbol);
			RegisterIntConstant(temporaryValue.x);
			RegisterIntConstant(temporaryValue.y);
			RegisterIntConstant(temporaryValue.z);
			RegisterIntConstant(temporaryValue.w);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_UINT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueInt(symbol);
			RegisterUintConstant(temporaryValue.x);
			RegisterUintConstant(temporaryValue.y);
			RegisterUintConstant(temporaryValue.z);
			RegisterUintConstant(temporaryValue.w);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_USHORT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueInt(symbol);
			RegisterUshortConstant(temporaryValue.x);
			RegisterUshortConstant(temporaryValue.y);
			RegisterUshortConstant(temporaryValue.z);
			RegisterUshortConstant(temporaryValue.w);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_UCHAR4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueInt(symbol);
			RegisterUcharConstant(temporaryValue.x);
			RegisterUcharConstant(temporaryValue.y);
			RegisterUcharConstant(temporaryValue.z);
			RegisterUcharConstant(temporaryValue.w);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_BOOL4:
			//No need to register constants
			break;
		default:
			assert(false);
			break;
		}
	}
}

void CSpirvShaderGenerator::DeclareTemporaryValueIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEMPORARY) continue;
		uint32 temporaryValueId = AllocateId();
		switch(symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValue(symbol);
			uint32 valueXId = m_floatConstantIds[temporaryValue.x];
			uint32 valueYId = m_floatConstantIds[temporaryValue.y];
			uint32 valueZId = m_floatConstantIds[temporaryValue.z];
			uint32 valueWId = m_floatConstantIds[temporaryValue.w];
			WriteOp(spv::OpConstantComposite, m_float4TypeId, temporaryValueId, valueXId, valueYId, valueZId, valueWId);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_INT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueInt(symbol);
			uint32 valueXId = m_intConstantIds[temporaryValue.x];
			uint32 valueYId = m_intConstantIds[temporaryValue.y];
			uint32 valueZId = m_intConstantIds[temporaryValue.z];
			uint32 valueWId = m_intConstantIds[temporaryValue.w];
			WriteOp(spv::OpConstantComposite, m_int4TypeId, temporaryValueId, valueXId, valueYId, valueZId, valueWId);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_UINT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueInt(symbol);
			uint32 valueXId = m_uintConstantIds[temporaryValue.x];
			uint32 valueYId = m_uintConstantIds[temporaryValue.y];
			uint32 valueZId = m_uintConstantIds[temporaryValue.z];
			uint32 valueWId = m_uintConstantIds[temporaryValue.w];
			WriteOp(spv::OpConstantComposite, m_uint4TypeId, temporaryValueId, valueXId, valueYId, valueZId, valueWId);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_USHORT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueInt(symbol);
			uint32 valueXId = m_ushortConstantIds[temporaryValue.x];
			uint32 valueYId = m_ushortConstantIds[temporaryValue.y];
			uint32 valueZId = m_ushortConstantIds[temporaryValue.z];
			uint32 valueWId = m_ushortConstantIds[temporaryValue.w];
			WriteOp(spv::OpConstantComposite, m_ushort4TypeId, temporaryValueId, valueXId, valueYId, valueZId, valueWId);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_UCHAR4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueInt(symbol);
			uint32 valueXId = m_ucharConstantIds[temporaryValue.x];
			uint32 valueYId = m_ucharConstantIds[temporaryValue.y];
			uint32 valueZId = m_ucharConstantIds[temporaryValue.z];
			uint32 valueWId = m_ucharConstantIds[temporaryValue.w];
			WriteOp(spv::OpConstantComposite, m_uchar4TypeId, temporaryValueId, valueXId, valueYId, valueZId, valueWId);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_BOOL4:
		{
			auto value = m_shaderBuilder.GetTemporaryValueBool(symbol);
			WriteOp(spv::OpConstantComposite, m_bool4TypeId, temporaryValueId,
			        value.x ? m_boolConstantTrueId : m_boolConstantFalseId,
			        value.y ? m_boolConstantTrueId : m_boolConstantFalseId,
			        value.z ? m_boolConstantTrueId : m_boolConstantFalseId,
			        value.w ? m_boolConstantTrueId : m_boolConstantFalseId);
		}
		break;
		default:
			assert(false);
			break;
		}
		m_temporaryValueIds[symbol.index] = temporaryValueId;
	}
}

void CSpirvShaderGenerator::AllocateVariablePointerIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_VARIABLE) continue;
		assert(m_variablePointerIds.find(symbol.index) == std::end(m_variablePointerIds));
		auto pointerId = AllocateId();
		m_variablePointerIds[symbol.index] = pointerId;
	}
}

void CSpirvShaderGenerator::WriteVariablePointerNames()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_VARIABLE) continue;
		assert(m_variablePointerIds.find(symbol.index) != std::end(m_variablePointerIds));
		auto pointerId = m_variablePointerIds[symbol.index];
		auto variableName = m_shaderBuilder.GetVariableName(symbol);
		WriteOp(spv::OpName, pointerId, variableName.c_str());
	}
}

void CSpirvShaderGenerator::DeclareVariablePointerIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_VARIABLE) continue;
		assert(m_variablePointerIds.find(symbol.index) != std::end(m_variablePointerIds));
		auto pointerId = m_variablePointerIds[symbol.index];
		switch(symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			WriteOp(spv::OpVariable, m_functionFloat4PointerTypeId, pointerId, spv::StorageClassFunction);
			break;
		case CShaderBuilder::SYMBOL_TYPE_INT4:
			WriteOp(spv::OpVariable, m_functionInt4PointerTypeId, pointerId, spv::StorageClassFunction);
			break;
		case CShaderBuilder::SYMBOL_TYPE_UINT4:
			WriteOp(spv::OpVariable, m_functionUint4PointerTypeId, pointerId, spv::StorageClassFunction);
			break;
		case CShaderBuilder::SYMBOL_TYPE_BOOL4:
			WriteOp(spv::OpVariable, m_functionBool4PointerTypeId, pointerId, spv::StorageClassFunction);
			break;
		default:
			assert(false);
			break;
		}
	}
}

void CSpirvShaderGenerator::AllocateUniformStructsIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
		auto& structInfo = m_structInfos[symbol.unit];
		uint32 memberIndex = structInfo.memberIndex++;
		structInfo.memberIndices[symbol.index] = memberIndex;
		switch(symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			structInfo.components.push_back(m_float4TypeId);
			break;
		case CShaderBuilder::SYMBOL_TYPE_INT4:
			structInfo.components.push_back(m_int4TypeId);
			break;
		case CShaderBuilder::SYMBOL_TYPE_UINT4:
			structInfo.components.push_back(m_uint4TypeId);
			break;
		case CShaderBuilder::SYMBOL_TYPE_MATRIX:
			structInfo.components.push_back(m_matrix44TypeId);
			break;
		case CShaderBuilder::SYMBOL_TYPE_ARRAYUINT:
			structInfo.components.push_back(m_uintArrayTypeId);
			structInfo.isBufferBlock = true;
			break;
		case CShaderBuilder::SYMBOL_TYPE_ARRAYUCHAR:
			structInfo.components.push_back(m_ucharArrayTypeId);
			structInfo.isBufferBlock = true;
			break;
		case CShaderBuilder::SYMBOL_TYPE_ARRAYUSHORT:
			structInfo.components.push_back(m_ushortArrayTypeId);
			structInfo.isBufferBlock = true;
			break;
		default:
			assert(false);
			break;
		}
	}

	for(auto& structInfoPair : m_structInfos)
	{
		auto& structInfo = structInfoPair.second;
		structInfo.typeId = AllocateId();
		structInfo.pointerTypeId = AllocateId();
		structInfo.variableId = AllocateId();
	}
}

void CSpirvShaderGenerator::WriteUniformStructNames()
{
	for(auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
		const auto& structInfo = m_structInfos[symbol.unit];
		auto memberIndexIterator = structInfo.memberIndices.find(symbol.index);
		assert(memberIndexIterator != std::end(structInfo.memberIndices));
		auto memberIndex = memberIndexIterator->second;
		auto uniformName = m_shaderBuilder.GetUniformName(symbol);
		WriteOp(spv::OpMemberName, structInfo.typeId, memberIndex, uniformName.c_str());
	}
}

void CSpirvShaderGenerator::DecorateUniformStructIds()
{
	for(auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
		auto& structInfo = m_structInfos[symbol.unit];
		auto memberIndex = structInfo.memberIndices[symbol.index];
		WriteOp(spv::OpMemberDecorate, structInfo.typeId, memberIndex, spv::DecorationOffset, structInfo.currentOffset);
		if(symbol.attributes & SYMBOL_ATTRIBUTE_COHERENT)
		{
			WriteOp(spv::OpMemberDecorate, structInfo.typeId, memberIndex, spv::DecorationCoherent);
		}
		if(symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX)
		{
			WriteOp(spv::OpMemberDecorate, structInfo.typeId, memberIndex, spv::DecorationColMajor);
			WriteOp(spv::OpMemberDecorate, structInfo.typeId, memberIndex, spv::DecorationMatrixStride, 16);
		}
		RegisterIntConstant(memberIndex);
		//Assuming std430 layout
		switch(symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		case CShaderBuilder::SYMBOL_TYPE_INT4:
		case CShaderBuilder::SYMBOL_TYPE_UINT4:
			//sizeof(float) * 4
			structInfo.currentOffset += 16;
			break;
		case CShaderBuilder::SYMBOL_TYPE_MATRIX:
			//sizeof(float) * 16
			structInfo.currentOffset += 64;
			break;
		case CShaderBuilder::SYMBOL_TYPE_ARRAYUINT:
		case CShaderBuilder::SYMBOL_TYPE_ARRAYUCHAR:
		case CShaderBuilder::SYMBOL_TYPE_ARRAYUSHORT:
			//This needs to be the last element of a struct
			break;
		default:
			assert(false);
			break;
		}
	}

	for(const auto& structInfoPair : m_structInfos)
	{
		const auto& structInfo = structInfoPair.second;
		auto structUnit = structInfoPair.first;
		if(structInfo.isBufferBlock)
		{
			WriteOp(spv::OpDecorate, structInfo.typeId, spv::DecorationBufferBlock);
		}
		else
		{
			WriteOp(spv::OpDecorate, structInfo.typeId, spv::DecorationBlock);
		}
		if(structUnit != Nuanceur::UNIFORM_UNIT_PUSHCONSTANT)
		{
			WriteOp(spv::OpDecorate, structInfo.variableId, spv::DecorationDescriptorSet, 0);
			WriteOp(spv::OpDecorate, structInfo.variableId, spv::DecorationBinding, structUnit);
		}
	}
}

void CSpirvShaderGenerator::DeclareUniformStructIds()
{
	if(m_structInfos.empty()) return;

	for(const auto& structInfoPair : m_structInfos)
	{
		const auto& structInfo = structInfoPair.second;
		auto structUnit = structInfoPair.first;

		WriteOp(spv::OpTypeStruct, structInfo.typeId, structInfo.components);
		if(structUnit == Nuanceur::UNIFORM_UNIT_PUSHCONSTANT)
		{
			WriteOp(spv::OpTypePointer, structInfo.pointerTypeId, spv::StorageClassPushConstant, structInfo.typeId);
		}
		else
		{
			WriteOp(spv::OpTypePointer, structInfo.pointerTypeId, spv::StorageClassUniform, structInfo.typeId);
		}
	}

	WriteOp(spv::OpTypePointer, m_pushFloat4PointerTypeId, spv::StorageClassPushConstant, m_float4TypeId);
	WriteOp(spv::OpTypePointer, m_pushInt4PointerTypeId, spv::StorageClassPushConstant, m_int4TypeId);
	WriteOp(spv::OpTypePointer, m_pushMatrix44PointerTypeId, spv::StorageClassPushConstant, m_matrix44TypeId);

	WriteOp(spv::OpTypePointer, m_uniformFloat4PointerTypeId, spv::StorageClassUniform, m_float4TypeId);
	WriteOp(spv::OpTypePointer, m_uniformInt4PointerTypeId, spv::StorageClassUniform, m_int4TypeId);
	WriteOp(spv::OpTypePointer, m_uniformUintPtrId, spv::StorageClassUniform, m_uintTypeId);
	if(m_has8BitInt)
		WriteOp(spv::OpTypePointer, m_uniformUint8PtrId, spv::StorageClassUniform, m_ucharTypeId);

	if(m_has16BitInt)
		WriteOp(spv::OpTypePointer, m_uniformUint16PtrId, spv::StorageClassUniform, m_ushortTypeId);
}

void CSpirvShaderGenerator::AllocateTextureIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEXTURE) continue;
		assert(m_texturePointerIds.find(symbol.unit) == std::end(m_texturePointerIds));
		auto pointerId = AllocateId();
		m_texturePointerIds[symbol.unit] = pointerId;
	}
}

void CSpirvShaderGenerator::DecorateTextureIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEXTURE) continue;
		assert(m_texturePointerIds.find(symbol.unit) != std::end(m_texturePointerIds));
		auto pointerId = m_texturePointerIds[symbol.unit];
		WriteOp(spv::OpDecorate, pointerId, spv::DecorationDescriptorSet, 0);
		WriteOp(spv::OpDecorate, pointerId, spv::DecorationBinding, symbol.unit);
		if(
		    (symbol.type == CShaderBuilder::SYMBOL_TYPE_SUBPASSINPUT) ||
		    (symbol.type == CShaderBuilder::SYMBOL_TYPE_SUBPASSINPUTUINT))
		{
			WriteOp(spv::OpDecorate, pointerId, spv::DecorationInputAttachmentIndex, symbol.index);
		}
	}
}

void CSpirvShaderGenerator::DeclareTextureIds()
{
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEXTURE) continue;
		assert(m_texturePointerIds.find(symbol.unit) != std::end(m_texturePointerIds));
		auto pointerId = m_texturePointerIds[symbol.unit];
		switch(symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_TEXTURE2D:
			assert(m_sampledImageSamplerPointerTypeId != EMPTY_ID);
			WriteOp(spv::OpVariable, m_sampledImageSamplerPointerTypeId, pointerId, spv::StorageClassUniformConstant);
			break;
		case CShaderBuilder::SYMBOL_TYPE_IMAGE2DUINT:
			assert(m_storageImage2DPointerTypeId != EMPTY_ID);
			WriteOp(spv::OpVariable, m_storageImage2DPointerTypeId, pointerId, spv::StorageClassUniformConstant);
			break;
		case CShaderBuilder::SYMBOL_TYPE_SUBPASSINPUT:
			assert(m_subpassInputPointerTypeId != EMPTY_ID);
			WriteOp(spv::OpVariable, m_subpassInputPointerTypeId, pointerId, spv::StorageClassUniformConstant);
			break;
		case CShaderBuilder::SYMBOL_TYPE_SUBPASSINPUTUINT:
			assert(m_subpassInputUintPointerTypeId != EMPTY_ID);
			WriteOp(spv::OpVariable, m_subpassInputUintPointerTypeId, pointerId, spv::StorageClassUniformConstant);
			break;
		default:
			assert(false);
			break;
		}
	}
}

void CSpirvShaderGenerator::RegisterFloatConstant(float value)
{
	if(m_floatConstantIds.find(value) != std::end(m_floatConstantIds)) return;
	m_floatConstantIds[value] = AllocateId();
}

void CSpirvShaderGenerator::RegisterIntConstant(int32 value)
{
	if(m_intConstantIds.find(value) != std::end(m_intConstantIds)) return;
	m_intConstantIds[value] = AllocateId();
}

void CSpirvShaderGenerator::RegisterUintConstant(uint32 value)
{
	if(m_uintConstantIds.find(value) != std::end(m_uintConstantIds)) return;
	m_uintConstantIds[value] = AllocateId();
}

void CSpirvShaderGenerator::RegisterUshortConstant(uint32 value)
{
	assert(value < 0x10000);
	if(m_ushortConstantIds.find(value) != std::end(m_ushortConstantIds)) return;
	m_ushortConstantIds[value] = AllocateId();
}

void CSpirvShaderGenerator::RegisterUcharConstant(uint32 value)
{
	assert(value < 0x100);
	if(m_ucharConstantIds.find(value) != std::end(m_ucharConstantIds)) return;
	m_ucharConstantIds[value] = AllocateId();
}

uint32 CSpirvShaderGenerator::LoadFromSymbol(const CShaderBuilder::SYMBOLREF& srcRef)
{
	uint32 srcId = 0;
	switch(srcRef.symbol.location)
	{
	case CShaderBuilder::SYMBOL_LOCATION_INPUT:
	{
		srcId = AllocateId();
		assert(m_inputPointerIds.find(srcRef.symbol.index) != std::end(m_inputPointerIds));
		auto pointerId = m_inputPointerIds[srcRef.symbol.index];
		auto semantic = m_shaderBuilder.GetInputSemantic(srcRef.symbol);
		switch(semantic.type)
		{
		case Nuanceur::SEMANTIC_SYSTEM_VERTEXINDEX:
		{
			assert(m_intConstantIds.find(0) != std::end(m_intConstantIds));
			uint32 tempId = AllocateId();
			WriteOp(spv::OpLoad, m_intTypeId, tempId, pointerId);
			WriteOp(spv::OpCompositeConstruct, m_int4TypeId, srcId, tempId, m_intConstantIds[0], m_intConstantIds[0], m_intConstantIds[0]);
		}
		break;
		case Nuanceur::SEMANTIC_SYSTEM_GIID:
		{
			assert(m_intConstantIds.find(0) != std::end(m_intConstantIds));
			uint32 tempId = AllocateId();
			WriteOp(spv::OpLoad, m_int3TypeId, tempId, pointerId);
			WriteOp(spv::OpCompositeConstruct, m_int4TypeId, srcId, tempId, m_intConstantIds[0]);
		}
		break;
		default:
			switch(srcRef.symbol.type)
			{
			case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
				WriteOp(spv::OpLoad, m_float4TypeId, srcId, pointerId);
				break;
			case CShaderBuilder::SYMBOL_TYPE_UINT4:
				WriteOp(spv::OpLoad, m_uint4TypeId, srcId, pointerId);
				break;
			default:
				assert(false);
				break;
			}
			break;
		}
	}
	break;
	case CShaderBuilder::SYMBOL_LOCATION_OUTPUT:
	{
		assert(srcRef.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4);
		srcId = AllocateId();
		auto pointerId = GetOutputPointerId(srcRef);
		WriteOp(spv::OpLoad, m_float4TypeId, srcId, pointerId);
	}
	break;
	case CShaderBuilder::SYMBOL_LOCATION_UNIFORM:
	{
		assert(!m_structInfos.empty());
		srcId = AllocateId();
		auto memberPointerId = AllocateId();

		bool pushCstPtr = (srcRef.symbol.unit == Nuanceur::UNIFORM_UNIT_PUSHCONSTANT);
		assert(m_structInfos.find(srcRef.symbol.unit) != std::end(m_structInfos));
		auto structInfo = m_structInfos[srcRef.symbol.unit];
		auto memberIndex = structInfo.memberIndices[srcRef.symbol.index];
		assert(m_intConstantIds.find(memberIndex) != m_intConstantIds.end());
		auto memberIdxConstantId = m_intConstantIds[memberIndex];

		switch(srcRef.symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			WriteOp(spv::OpAccessChain, pushCstPtr ? m_pushFloat4PointerTypeId : m_uniformFloat4PointerTypeId, memberPointerId, structInfo.variableId, memberIdxConstantId);
			WriteOp(spv::OpLoad, m_float4TypeId, srcId, memberPointerId);
			break;
		case CShaderBuilder::SYMBOL_TYPE_INT4:
			WriteOp(spv::OpAccessChain, pushCstPtr ? m_pushInt4PointerTypeId : m_uniformInt4PointerTypeId, memberPointerId, structInfo.variableId, memberIdxConstantId);
			WriteOp(spv::OpLoad, m_int4TypeId, srcId, memberPointerId);
			break;
		case CShaderBuilder::SYMBOL_TYPE_MATRIX:
			assert(pushCstPtr);
			WriteOp(spv::OpAccessChain, m_pushMatrix44PointerTypeId, memberPointerId, structInfo.variableId, memberIdxConstantId);
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
		uint32 imageTypeId = EMPTY_ID;
		switch(srcRef.symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_TEXTURE2D:
			assert(m_sampledImageSamplerTypeId != EMPTY_ID);
			imageTypeId = m_sampledImageSamplerTypeId;
			break;
		case CShaderBuilder::SYMBOL_TYPE_IMAGE2DUINT:
			assert(m_storageImage2DTypeId != EMPTY_ID);
			imageTypeId = m_storageImage2DTypeId;
			break;
		case CShaderBuilder::SYMBOL_TYPE_SUBPASSINPUT:
			assert(m_subpassInputTypeId != EMPTY_ID);
			imageTypeId = m_subpassInputTypeId;
			break;
		case CShaderBuilder::SYMBOL_TYPE_SUBPASSINPUTUINT:
			assert(m_subpassInputUintTypeId != EMPTY_ID);
			imageTypeId = m_subpassInputUintTypeId;
			break;
		default:
			assert(false);
			break;
		}
		srcId = AllocateId();
		assert(m_texturePointerIds.find(srcRef.symbol.unit) != std::end(m_texturePointerIds));
		auto pointerId = m_texturePointerIds[srcRef.symbol.unit];
		WriteOp(spv::OpLoad, imageTypeId, srcId, pointerId);
	}
	break;
	case CShaderBuilder::SYMBOL_LOCATION_TEMPORARY:
	{
		assert(m_temporaryValueIds.find(srcRef.symbol.index) != std::end(m_temporaryValueIds));
		srcId = m_temporaryValueIds[srcRef.symbol.index];
	}
	break;
	case CShaderBuilder::SYMBOL_LOCATION_VARIABLE:
	{
		srcId = AllocateId();
		assert(m_variablePointerIds.find(srcRef.symbol.index) != std::end(m_variablePointerIds));
		auto pointerId = m_variablePointerIds[srcRef.symbol.index];
		switch(srcRef.symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			WriteOp(spv::OpLoad, m_float4TypeId, srcId, pointerId);
			break;
		case CShaderBuilder::SYMBOL_TYPE_INT4:
			WriteOp(spv::OpLoad, m_int4TypeId, srcId, pointerId);
			break;
		case CShaderBuilder::SYMBOL_TYPE_UINT4:
			WriteOp(spv::OpLoad, m_uint4TypeId, srcId, pointerId);
			break;
		case CShaderBuilder::SYMBOL_TYPE_BOOL4:
			WriteOp(spv::OpLoad, m_bool4TypeId, srcId, pointerId);
			break;
		default:
			assert(false);
			break;
		}
	}
	break;
	default:
		assert(false);
		break;
	}
	assert(srcId != 0);
	if(!IsIdentitySwizzle(srcRef.swizzle))
	{
		uint32 srcType = EMPTY_ID;
		switch(srcRef.symbol.type)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			srcType = m_float4TypeId;
			break;
		case CShaderBuilder::SYMBOL_TYPE_INT4:
			srcType = m_int4TypeId;
			break;
		case CShaderBuilder::SYMBOL_TYPE_UINT4:
			srcType = m_uint4TypeId;
			break;
		default:
			assert(false);
			break;
		}
		auto prevId = srcId;
		std::array<uint32, 4> components = {0, 0, 0, 0};
		srcId = AllocateId();
		switch(GetSwizzleElementCount(srcRef.swizzle))
		{
		case 1:
			components = {GetSwizzleElement(srcRef.swizzle, 0)};
			break;
		case 2:
			components = {
			    GetSwizzleElement(srcRef.swizzle, 0),
			    GetSwizzleElement(srcRef.swizzle, 1)};
			break;
		case 3:
			components = {
			    GetSwizzleElement(srcRef.swizzle, 0),
			    GetSwizzleElement(srcRef.swizzle, 1),
			    GetSwizzleElement(srcRef.swizzle, 2)};
			break;
		case 4:
			components = {
			    GetSwizzleElement(srcRef.swizzle, 0),
			    GetSwizzleElement(srcRef.swizzle, 1),
			    GetSwizzleElement(srcRef.swizzle, 2),
			    GetSwizzleElement(srcRef.swizzle, 3)};
			break;
		default:
			assert(false);
			break;
		}
		WriteOp(spv::OpVectorShuffle, srcType, srcId, prevId, prevId,
		        components[0], components[1], components[2], components[3]);
	}
	assert(srcId != 0);
	return srcId;
}

void CSpirvShaderGenerator::StoreToSymbol(const CShaderBuilder::SYMBOLREF& dstRef, uint32 valueId)
{
	assert(IsMaskSwizzle(dstRef.swizzle));
	uint32 vectorTypeId =
	    [&]() {
		    switch(dstRef.symbol.type)
		    {
		    default:
			    assert(false);
			    [[fallthrough]];
		    case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			    return m_float4TypeId;
		    case CShaderBuilder::SYMBOL_TYPE_INT4:
			    return m_int4TypeId;
		    case CShaderBuilder::SYMBOL_TYPE_UINT4:
			    return m_uint4TypeId;
		    case CShaderBuilder::SYMBOL_TYPE_USHORT4:
			    return m_ushort4TypeId;
		    case CShaderBuilder::SYMBOL_TYPE_UCHAR4:
			    return m_uchar4TypeId;
		    case CShaderBuilder::SYMBOL_TYPE_BOOL4:
			    return m_bool4TypeId;
		    }
	    }();
	auto mixSrcAndDst = [&](uint32 srcValueId, uint32 dstValueId, SWIZZLE_TYPE dstSwizzle) {
		//Makes a new destination with src and dst, respecting dst swizzle
		uint32 resultId = AllocateId();
		std::array<uint32, 4> components = {0, 1, 2, 3};
		uint32 elemCount = GetSwizzleElementCount(dstSwizzle);
		for(int i = 0; i < elemCount; i++)
		{
			uint32 elem = GetSwizzleElement(dstSwizzle, i);
			components[elem] = 4 + i;
		}
		WriteOp(spv::OpVectorShuffle, vectorTypeId, resultId, dstValueId, srcValueId,
		        components[0], components[1], components[2], components[3]);
		return resultId;
	};

	switch(dstRef.symbol.location)
	{
	case CShaderBuilder::SYMBOL_LOCATION_OUTPUT:
	{
		auto pointerId = GetOutputPointerId(dstRef);
		auto outputSemantic = m_shaderBuilder.GetOutputSemantic(dstRef.symbol);
		if(outputSemantic.type == SEMANTIC_SYSTEM_POINTSIZE)
		{
			//Output is a scalar float and we need to extract the first element from the vector
			assert(dstRef.swizzle == SWIZZLE_X);
			auto scalarValueId = ExtractFloat4X(valueId);
			WriteOp(spv::OpStore, pointerId, scalarValueId);
		}
		else
		{
			//Output is a float4. If our swizzle elem size ain't 4
			//we need to shuffle the src with output value.
			//Input should already be properly ordered
			uint32 elemCount = GetSwizzleElementCount(dstRef.swizzle);
			switch(elemCount)
			{
			case 1:
			case 2:
			case 3:
			{
				uint32 dstValueId = AllocateId();
				WriteOp(spv::OpLoad, vectorTypeId, dstValueId, pointerId);
				uint32 swizzledValueId = mixSrcAndDst(valueId, dstValueId, dstRef.swizzle);
				WriteOp(spv::OpStore, pointerId, swizzledValueId);
			}
			break;
			case 4:
				assert(dstRef.swizzle == SWIZZLE_XYZW);
				WriteOp(spv::OpStore, pointerId, valueId);
				break;
			default:
				assert(false);
				break;
			}
		}
	}
	break;
	case CShaderBuilder::SYMBOL_LOCATION_TEMPORARY:
	{
		uint32 elemCount = GetSwizzleElementCount(dstRef.swizzle);
		switch(elemCount)
		{
		case 1:
		case 2:
		case 3:
		{
			uint32 dstValueId = m_temporaryValueIds[dstRef.symbol.index];
			uint32 swizzledValueId = mixSrcAndDst(valueId, dstValueId, dstRef.swizzle);
			m_temporaryValueIds[dstRef.symbol.index] = swizzledValueId;
		}
		break;
		case 4:
			assert(dstRef.swizzle == SWIZZLE_XYZW);
			//Replace current active id for that temporary symbol.
			m_temporaryValueIds[dstRef.symbol.index] = valueId;
			break;
		default:
			assert(false);
			break;
		}
	}
	break;
	case CShaderBuilder::SYMBOL_LOCATION_VARIABLE:
	{
		assert(m_variablePointerIds.find(dstRef.symbol.index) != std::end(m_variablePointerIds));
		auto pointerId = m_variablePointerIds[dstRef.symbol.index];
		uint32 elemCount = GetSwizzleElementCount(dstRef.swizzle);
		switch(elemCount)
		{
		case 1:
		case 2:
		case 3:
		{
			uint32 dstValueId = AllocateId();
			WriteOp(spv::OpLoad, vectorTypeId, dstValueId, pointerId);
			uint32 swizzledValueId = mixSrcAndDst(valueId, dstValueId, dstRef.swizzle);
			WriteOp(spv::OpStore, pointerId, swizzledValueId);
		}
		break;
		case 4:
			assert(dstRef.swizzle == SWIZZLE_XYZW);
			WriteOp(spv::OpStore, pointerId, valueId);
			break;
		default:
			assert(false);
			break;
		}
	}
	break;
	default:
		assert(false);
		break;
	}
}

std::pair<uint32, uint32> CSpirvShaderGenerator::GetStructAccessChainParams(const CShaderBuilder::SYMBOLREF& symRef)
{
	assert(m_structInfos.find(symRef.symbol.unit) != std::end(m_structInfos));
	auto structInfo = m_structInfos[symRef.symbol.unit];
	auto memberIndex = structInfo.memberIndices[symRef.symbol.index];
	assert(m_intConstantIds.find(memberIndex) != m_intConstantIds.end());
	auto memberIdxConstantId = m_intConstantIds[memberIndex];
	return std::make_pair(structInfo.variableId, memberIdxConstantId);
}

uint32 CSpirvShaderGenerator::ExtractFloat4X(uint32 float4VectorId)
{
	uint32 resultId = AllocateId();
	WriteOp(spv::OpCompositeExtract, m_floatTypeId, resultId, float4VectorId, 0);
	return resultId;
}

uint32 CSpirvShaderGenerator::GetResultType(CShaderBuilder::SYMBOL_TYPE symbolType) const
{
	switch(symbolType)
	{
	case CShaderBuilder::SYMBOL_TYPE_UINT4:
		return m_uint4TypeId;
	case CShaderBuilder::SYMBOL_TYPE_INT4:
		return m_int4TypeId;
	default:
		assert(false);
		[[fallthrough]];
	case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		return m_float4TypeId;
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
		return index;
		break;
	default:
		assert(false);
		break;
	}
	return 0;
}

bool CSpirvShaderGenerator::IsBuiltInOutput(Nuanceur::SEMANTIC semantic) const
{
	if(m_shaderType == SHADER_TYPE_VERTEX)
	{
		if(semantic == Nuanceur::SEMANTIC_SYSTEM_POSITION) return true;
		if(semantic == Nuanceur::SEMANTIC_SYSTEM_POINTSIZE) return true;
	}
	return false;
}

uint32 CSpirvShaderGenerator::MakeDefinedInt4Vector(uint32 srcValueId, SWIZZLE_TYPE swizzle)
{
	uint32 result = 0;
	//Make sure our whole vector contains sensible values since we're dividing
	//across 4 lanes. Issues can happen when 0 happens in one of the components.
	switch(GetSwizzleElementCount(swizzle))
	{
	case 1:
		result = AllocateId();
		WriteOp(spv::OpVectorShuffle, m_int4TypeId, result, srcValueId, srcValueId, 0, 0, 0, 0);
		break;
	case 2:
		result = AllocateId();
		WriteOp(spv::OpVectorShuffle, m_int4TypeId, result, srcValueId, srcValueId, 0, 1, 0, 1);
		break;
	default:
		assert(false);
		[[fallthrough]];
	case 4:
		result = srcValueId;
		break;
	}
	return result;
}

uint32 CSpirvShaderGenerator::AllocateId()
{
	return m_nextId++;
}

void CSpirvShaderGenerator::Add(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref);
	auto resultId = AllocateId();
	switch(symbolType)
	{
	case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		WriteOp(spv::OpFAdd, m_float4TypeId, resultId, src1Id, src2Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_INT4:
		WriteOp(spv::OpIAdd, m_int4TypeId, resultId, src1Id, src2Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_UINT4:
		WriteOp(spv::OpIAdd, m_uint4TypeId, resultId, src1Id, src2Id);
		break;
	default:
		assert(false);
		break;
	}
	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::BitwiseOp(spv::Op op, const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref);
	auto resultId = AllocateId();
	switch(symbolType)
	{
	case CShaderBuilder::SYMBOL_TYPE_INT4:
		WriteOp(op, m_int4TypeId, resultId, src1Id, src2Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_UINT4:
		WriteOp(op, m_uint4TypeId, resultId, src1Id, src2Id);
		break;
	default:
		assert(false);
		break;
	}
	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::BitwiseNot(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto resultId = AllocateId();
	switch(src1Ref.symbol.type)
	{
	case CShaderBuilder::SYMBOL_TYPE_INT4:
		WriteOp(spv::OpNot, m_int4TypeId, resultId, src1Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_UINT4:
		WriteOp(spv::OpNot, m_uint4TypeId, resultId, src1Id);
		break;
	default:
		assert(false);
		break;
	}
	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::Div(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref);
	auto resultId = AllocateId();
	switch(symbolType)
	{
	case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		WriteOp(spv::OpFDiv, m_float4TypeId, resultId, src1Id, src2Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_INT4:
	{
		uint32 dividerId = MakeDefinedInt4Vector(src2Id, src2Ref.swizzle);
		WriteOp(spv::OpSDiv, m_int4TypeId, resultId, src1Id, dividerId);
	}
	break;
	default:
		assert(false);
		break;
	}
	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::Mod(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref);
	assert(symbolType == CShaderBuilder::SYMBOL_TYPE_INT4);
	auto resultId = AllocateId();
	uint32 dividerId = MakeDefinedInt4Vector(src2Id, src2Ref.swizzle);
	WriteOp(spv::OpSMod, m_int4TypeId, resultId, src1Id, dividerId);
	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::LogicalOp(spv::Op op, const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref);
	assert(symbolType == CShaderBuilder::SYMBOL_TYPE_BOOL4);

	auto resultId = AllocateId();
	WriteOp(op, m_bool4TypeId, resultId, src1Id, src2Id);
	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::LogicalNot(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto resultId = AllocateId();
	WriteOp(spv::OpLogicalNot, m_bool4TypeId, resultId, src1Id);
	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::Sub(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto resultId = AllocateId();
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref);
	switch(symbolType)
	{
	case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		WriteOp(spv::OpFSub, m_float4TypeId, resultId, src1Id, src2Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_INT4:
		WriteOp(spv::OpISub, m_int4TypeId, resultId, src1Id, src2Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_UINT4:
		WriteOp(spv::OpISub, m_uint4TypeId, resultId, src1Id, src2Id);
		break;
	default:
		assert(false);
		break;
	}
	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::Clamp(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref,
                                  const CShaderBuilder::SYMBOLREF& src2Ref, const CShaderBuilder::SYMBOLREF& src3Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto src3Id = LoadFromSymbol(src3Ref);
	auto resultId = AllocateId();
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref, src3Ref);
	switch(symbolType)
	{
	case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		WriteOp(spv::OpExtInst, m_float4TypeId, resultId, m_glslStd450ExtInst, GLSLstd450::GLSLstd450FClamp, src1Id, src2Id, src3Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_INT4:
		WriteOp(spv::OpExtInst, m_int4TypeId, resultId, m_glslStd450ExtInst, GLSLstd450::GLSLstd450SClamp, src1Id, src2Id, src3Id);
		break;
	default:
		assert(false);
		break;
	}

	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::ClassifyFloat(spv::Op classifyOp, const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto resultId = AllocateId();
	WriteOp(classifyOp, m_bool4TypeId, resultId, src1Id);
	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::Dot(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto tempId = AllocateId();
	auto resultId = AllocateId();
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref);
	assert(symbolType == CShaderBuilder::SYMBOL_TYPE_FLOAT4);

	WriteOp(spv::OpDot, m_floatTypeId, tempId, src1Id, src2Id);
	WriteOp(spv::OpCompositeConstruct, m_float4TypeId, resultId, tempId, tempId, tempId, tempId);

	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::GlslStdOp(GLSLstd450 op, const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref)
{
	assert(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4);

	auto src1Id = LoadFromSymbol(src1Ref);
	auto resultId = AllocateId();

	WriteOp(spv::OpExtInst, m_float4TypeId, resultId, m_glslStd450ExtInst, op, src1Id);

	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::Min(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto resultId = AllocateId();
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref);
	switch(symbolType)
	{
	case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		WriteOp(spv::OpExtInst, m_float4TypeId, resultId, m_glslStd450ExtInst, GLSLstd450::GLSLstd450FMin, src1Id, src2Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_INT4:
		WriteOp(spv::OpExtInst, m_int4TypeId, resultId, m_glslStd450ExtInst, GLSLstd450::GLSLstd450SMin, src1Id, src2Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_UINT4:
		WriteOp(spv::OpExtInst, m_uint4TypeId, resultId, m_glslStd450ExtInst, GLSLstd450::GLSLstd450UMin, src1Id, src2Id);
		break;
	default:
		assert(false);
		break;
	}

	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::Max(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto resultId = AllocateId();
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref);
	switch(symbolType)
	{
	case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		WriteOp(spv::OpExtInst, m_float4TypeId, resultId, m_glslStd450ExtInst, GLSLstd450::GLSLstd450FMax, src1Id, src2Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_INT4:
		WriteOp(spv::OpExtInst, m_int4TypeId, resultId, m_glslStd450ExtInst, GLSLstd450::GLSLstd450SMax, src1Id, src2Id);
		break;
	case CShaderBuilder::SYMBOL_TYPE_UINT4:
		WriteOp(spv::OpExtInst, m_uint4TypeId, resultId, m_glslStd450ExtInst, GLSLstd450::GLSLstd450UMax, src1Id, src2Id);
		break;
	default:
		assert(false);
		break;
	}

	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::Mix(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref,
                                const CShaderBuilder::SYMBOLREF& src2Ref, const CShaderBuilder::SYMBOLREF& src3Ref)
{
	assert(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4);
	assert(src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4);

	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto src3Id = LoadFromSymbol(src3Ref);
	auto resultId = AllocateId();

	if(src3Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4)
	{
		WriteOp(spv::OpExtInst, m_float4TypeId, resultId, m_glslStd450ExtInst, GLSLstd450::GLSLstd450FMix,
		        src1Id, src2Id, src3Id);
	}
	else if(src3Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_BOOL4)
	{
		//Order for Select is different from typical Mix
		WriteOp(spv::OpSelect, m_float4TypeId, resultId, src3Id, src2Id, src1Id);
	}
	else
	{
		//Unknown operand
		assert(false);
	}

	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::Compare(CShaderBuilder::STATEMENT_OP op, const CShaderBuilder::SYMBOLREF& dstRef,
                                    const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	typedef std::pair<CShaderBuilder::STATEMENT_OP, spv::Op> CompareOpPair;

	static const CompareOpPair floatCompareOps[] =
	    {
	        {CShaderBuilder::STATEMENT_OP_COMPARE_EQ, spv::OpFOrdEqual},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_NE, spv::OpFOrdNotEqual},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_LT, spv::OpFOrdLessThan},
	    };

	static const CompareOpPair intCompareOps[] =
	    {
	        {CShaderBuilder::STATEMENT_OP_COMPARE_EQ, spv::OpIEqual},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_NE, spv::OpINotEqual},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_LT, spv::OpSLessThan},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_LE, spv::OpSLessThanEqual},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_GT, spv::OpSGreaterThan},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_GE, spv::OpSGreaterThanEqual},
	    };

	static const CompareOpPair uintCompareOps[] =
	    {
	        {CShaderBuilder::STATEMENT_OP_COMPARE_EQ, spv::OpIEqual},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_NE, spv::OpINotEqual},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_LT, spv::OpULessThan},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_LE, spv::OpULessThanEqual},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_GT, spv::OpUGreaterThan},
	        {CShaderBuilder::STATEMENT_OP_COMPARE_GE, spv::OpUGreaterThanEqual},
	    };

	auto opMatcher = [&](const CompareOpPair& compareOp) { return compareOp.first == op; };

	auto src1Id = LoadFromSymbol(src1Ref);
	auto src2Id = LoadFromSymbol(src2Ref);
	auto symbolType = GetCommonSymbolType(src1Ref, src2Ref);

	uint32 scalarTypeId =
	    [&]() {
		    switch(symbolType)
		    {
		    default:
			    assert(false);
			    [[fallthrough]];
		    case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			    return m_floatTypeId;
		    case CShaderBuilder::SYMBOL_TYPE_INT4:
			    return m_intTypeId;
		    case CShaderBuilder::SYMBOL_TYPE_UINT4:
			    return m_uintTypeId;
		    }
	    }();

	static const auto find_if_null =
	    [](auto beginIterator, auto endIterator, auto matcher) {
		    auto result = std::find_if(beginIterator, endIterator, matcher);
		    return (result == endIterator) ? nullptr : result;
	    };

	const auto compareOp =
	    [&]() -> const CompareOpPair* {
		switch(symbolType)
		{
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
			return find_if_null(std::begin(floatCompareOps), std::end(floatCompareOps), opMatcher);
		case CShaderBuilder::SYMBOL_TYPE_INT4:
			return find_if_null(std::begin(intCompareOps), std::end(intCompareOps), opMatcher);
		case CShaderBuilder::SYMBOL_TYPE_UINT4:
			return find_if_null(std::begin(uintCompareOps), std::end(uintCompareOps), opMatcher);
		default:
			assert(false);
			return nullptr;
		}
	}();
	auto opType = compareOp ? compareOp->second : spv::OpNop;
	assert(opType != spv::OpNop);

	auto resultId = AllocateId();
	WriteOp(opType, m_bool4TypeId, resultId, src1Id, src2Id);

	StoreToSymbol(dstRef, resultId);
}

void CSpirvShaderGenerator::AtomicImageOp(spv::Op op, const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref,
                                          const CShaderBuilder::SYMBOLREF& src2Ref, const CShaderBuilder::SYMBOLREF& src3Ref)
{
	assert(src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_INT4);
	assert(src3Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_UINT4);

	assert(m_intConstantIds.find(spv::ScopeDevice) != std::end(m_intConstantIds));
	assert(m_intConstantIds.find(spv::MemorySemanticsMaskNone) != std::end(m_intConstantIds));

	auto scopeId = m_intConstantIds[spv::ScopeDevice];
	auto semanticsId = m_intConstantIds[spv::MemorySemanticsMaskNone];

	if(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_IMAGE2DUINT)
	{
		assert(m_texturePointerIds.find(src1Ref.symbol.unit) != std::end(m_texturePointerIds));
		auto imagePointerId = m_texturePointerIds[src1Ref.symbol.unit];

		auto coordId = LoadFromSymbol(src2Ref);
		auto valueId = LoadFromSymbol(src3Ref);

		assert(m_intConstantIds.find(0) != std::end(m_intConstantIds));

		auto imageSample0Id = m_intConstantIds[0];

		auto texelPtrId = AllocateId();
		auto resultId = AllocateId();
		auto cvtCoordId = AllocateId();
		auto cvtValueId = AllocateId();

		assert(m_imageUintPtrId != EMPTY_ID);

		//Build some temporary values
		WriteOp(spv::OpVectorShuffle, m_int2TypeId, cvtCoordId, coordId, coordId, 0, 1);
		WriteOp(spv::OpCompositeExtract, m_uintTypeId, cvtValueId, valueId, 0);

		WriteOp(spv::OpImageTexelPointer, m_imageUintPtrId, texelPtrId, imagePointerId, cvtCoordId, imageSample0Id);
		WriteOp(op, m_uintTypeId, resultId, texelPtrId, scopeId, semanticsId, cvtValueId);
	}
	else if(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_ARRAYUINT)
	{
		auto bufferAccessParams = GetStructAccessChainParams(src1Ref);
		auto src1Id = AllocateId();
		auto src2Id = LoadFromSymbol(src2Ref);
		auto src3Id = LoadFromSymbol(src3Ref);
		auto indexId = AllocateId();
		auto resultId = AllocateId();

		auto cvtValueId = AllocateId();
		WriteOp(spv::OpCompositeExtract, m_uintTypeId, cvtValueId, src3Id, 0);

		WriteOp(spv::OpCompositeExtract, m_intTypeId, indexId, src2Id, 0);
		WriteOp(spv::OpAccessChain, m_uniformUintPtrId, src1Id, bufferAccessParams.first, bufferAccessParams.second, indexId);
		WriteOp(op, m_uintTypeId, resultId, src1Id, scopeId, semanticsId, cvtValueId);
	}
}

void CSpirvShaderGenerator::Load(const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref)
{
	assert(src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_INT4);

	if(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_ARRAYUINT)
	{
		assert(dstRef.symbol.type == CShaderBuilder::SYMBOL_TYPE_UINT4);

		auto bufferAccessParams = GetStructAccessChainParams(src1Ref);
		auto src1Id = AllocateId();
		auto src2Id = LoadFromSymbol(src2Ref);
		auto tempId = AllocateId();
		auto indexId = AllocateId();
		auto resultId = AllocateId();

		assert(m_uintConstantIds.find(0) != std::end(m_uintConstantIds));
		auto zeroConstantId = m_uintConstantIds[0];

		WriteOp(spv::OpCompositeExtract, m_intTypeId, indexId, src2Id, 0);
		WriteOp(spv::OpAccessChain, m_uniformUintPtrId, src1Id, bufferAccessParams.first, bufferAccessParams.second, indexId);
		WriteOp(spv::OpLoad, m_uintTypeId, tempId, src1Id);
		WriteOp(spv::OpCompositeConstruct, m_uint4TypeId, resultId, tempId, zeroConstantId, zeroConstantId, zeroConstantId);
		StoreToSymbol(dstRef, resultId);
	}
	else if(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_IMAGE2DUINT)
	{
		assert(dstRef.symbol.type == CShaderBuilder::SYMBOL_TYPE_UINT4);

		auto src1Id = LoadFromSymbol(src1Ref);
		auto src2Id = LoadFromSymbol(src2Ref);
		auto resultId = AllocateId();
		auto cvtCoordId = AllocateId();
		WriteOp(spv::OpVectorShuffle, m_int2TypeId, cvtCoordId, src2Id, src2Id, 0, 1);
		WriteOp(spv::OpImageRead, m_uint4TypeId, resultId, src1Id, cvtCoordId);
		StoreToSymbol(dstRef, resultId);
	}
	else if(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_SUBPASSINPUT)
	{
		assert(dstRef.symbol.type == CShaderBuilder::SYMBOL_TYPE_FLOAT4);

		auto src1Id = LoadFromSymbol(src1Ref);
		auto src2Id = LoadFromSymbol(src2Ref);
		auto resultId = AllocateId();
		WriteOp(spv::OpImageRead, m_float4TypeId, resultId, src1Id, src2Id);
		StoreToSymbol(dstRef, resultId);
	}
	else if(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_SUBPASSINPUTUINT)
	{
		assert(dstRef.symbol.type == CShaderBuilder::SYMBOL_TYPE_UINT4);

		auto src1Id = LoadFromSymbol(src1Ref);
		auto src2Id = LoadFromSymbol(src2Ref);
		auto resultId = AllocateId();
		WriteOp(spv::OpImageRead, m_uint4TypeId, resultId, src1Id, src2Id);
		StoreToSymbol(dstRef, resultId);
	}
	else
	{
		assert(false);
	}
}

void CSpirvShaderGenerator::Store(const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref, const CShaderBuilder::SYMBOLREF& src3Ref)
{
	assert(src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_INT4);
	assert(src3Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_UINT4);
	if(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_ARRAYUINT)
	{
		auto bufferAccessParams = GetStructAccessChainParams(src1Ref);
		auto src1Id = AllocateId();
		auto src2Id = LoadFromSymbol(src2Ref);
		auto src3Id = LoadFromSymbol(src3Ref);
		auto valueId = AllocateId();
		auto indexId = AllocateId();

		WriteOp(spv::OpCompositeExtract, m_intTypeId, indexId, src2Id, 0);
		WriteOp(spv::OpCompositeExtract, m_uintTypeId, valueId, src3Id, 0);
		WriteOp(spv::OpAccessChain, m_uniformUintPtrId, src1Id, bufferAccessParams.first, bufferAccessParams.second, indexId);
		WriteOp(spv::OpStore, src1Id, valueId);
	}
	else
	{
		assert(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_IMAGE2DUINT);
		auto src1Id = LoadFromSymbol(src1Ref);
		auto src2Id = LoadFromSymbol(src2Ref);
		auto src3Id = LoadFromSymbol(src3Ref);
		WriteOp(spv::OpImageWrite, src1Id, src2Id, src3Id);
	}
}

void CSpirvShaderGenerator::Store16(const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref, const CShaderBuilder::SYMBOLREF& src3Ref)
{
	assert(src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_INT4);
	assert(src3Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_USHORT4);
	assert(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_ARRAYUSHORT);

	auto bufferAccessParams = GetStructAccessChainParams(src1Ref);
	auto src1Id = AllocateId();
	auto src2Id = LoadFromSymbol(src2Ref);
	auto src3Id = LoadFromSymbol(src3Ref);
	auto valueId = AllocateId();
	auto indexId = AllocateId();

	WriteOp(spv::OpCompositeExtract, m_intTypeId, indexId, src2Id, 0);
	WriteOp(spv::OpCompositeExtract, m_ushortTypeId, valueId, src3Id, 0);

	WriteOp(spv::OpAccessChain, m_uniformUint16PtrId, src1Id, bufferAccessParams.first, bufferAccessParams.second, indexId);
	WriteOp(spv::OpStore, src1Id, valueId);
}

void CSpirvShaderGenerator::Store8(const CShaderBuilder::SYMBOLREF& src1Ref, const CShaderBuilder::SYMBOLREF& src2Ref, const CShaderBuilder::SYMBOLREF& src3Ref)
{
	assert(src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_INT4);
	assert(src3Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_UCHAR4);
	assert(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_ARRAYUCHAR);

	auto bufferAccessParams = GetStructAccessChainParams(src1Ref);
	auto src1Id = AllocateId();
	auto src2Id = LoadFromSymbol(src2Ref);
	auto src3Id = LoadFromSymbol(src3Ref);
	auto valueId = AllocateId();
	auto indexId = AllocateId();

	WriteOp(spv::OpCompositeExtract, m_intTypeId, indexId, src2Id, 0);
	WriteOp(spv::OpCompositeExtract, m_ucharTypeId, valueId, src3Id, 0);

	WriteOp(spv::OpAccessChain, m_uniformUint8PtrId, src1Id, bufferAccessParams.first, bufferAccessParams.second, indexId);
	WriteOp(spv::OpStore, src1Id, valueId);
}
