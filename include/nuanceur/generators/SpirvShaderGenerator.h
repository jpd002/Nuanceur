#pragma once

#include <set>
#include <map>
#include <cstring>
#include "nuanceur/builder/ShaderBuilder.h"
#include "Stream.h"
#include "../external/vulkan/spirv.hpp"

namespace Nuanceur
{
	namespace SpirvOpConverter
	{
		typedef std::vector<uint32> ConvertedParamArray;

		template <typename ParamType>
		static typename std::enable_if<std::is_integral<ParamType>::value>::type
		ConvertParam(ConvertedParamArray& convertedParams, const ParamType& param)
		{
			convertedParams.push_back(static_cast<uint32>(param));
		}

		template <typename ParamType>
		static typename std::enable_if<std::is_enum<ParamType>::value>::type
		ConvertParam(ConvertedParamArray& convertedParams, const ParamType& param)
		{
			convertedParams.push_back(static_cast<uint32>(param));
		}

		static void ConvertParam(ConvertedParamArray& convertedParams, const char* param)
		{
			uint32 stringLength = static_cast<uint32>(strlen(param)) + 1;
			uint32 valueCount = (stringLength + 3) / 4;
			for(uint32 i = 0; i < valueCount; i++)
			{
				assert(stringLength != 0);
				uint32 value = 0;
				if(stringLength > 0) value |= static_cast<uint32>(param[0] <<  0);
				if(stringLength > 1) value |= static_cast<uint32>(param[1] <<  8);
				if(stringLength > 2) value |= static_cast<uint32>(param[2] << 16);
				if(stringLength > 3) value |= static_cast<uint32>(param[3] << 24);
				convertedParams.push_back(value);
				param += 4;
				stringLength -= 4;
			}
		}

		static void ConvertParam(ConvertedParamArray& convertedParams, const std::vector<uint32>& param)
		{
			convertedParams.insert(convertedParams.end(), param.begin(), param.end());
		}

		static void ConvertParam(ConvertedParamArray& convertedParams, float param)
		{
			uint32 convertedValue = *reinterpret_cast<uint32*>(&param);
			convertedParams.push_back(convertedValue);
		}

		template <typename ParamType>
		static void ConvertParams(ConvertedParamArray& convertedParams, ParamType&& param)
		{
			ConvertParam(convertedParams, param);
		}

		template <typename ParamType, typename... OtherParamTypes>
		static void ConvertParams(ConvertedParamArray& convertedParams, ParamType&& param, OtherParamTypes &&...otherParams)
		{
			ConvertParam(convertedParams, param);
			ConvertParams(convertedParams, std::forward<OtherParamTypes>(otherParams)...);
		}
	}

	class CSpirvShaderGenerator
	{
	public:
		enum SHADER_TYPE
		{
			SHADER_TYPE_VERTEX,
			SHADER_TYPE_FRAGMENT,
			SHADER_TYPE_COMPUTE
		};

		static void Generate(Framework::CStream&, const CShaderBuilder&, SHADER_TYPE);

	private:
		        CSpirvShaderGenerator(Framework::CStream&, const CShaderBuilder&, SHADER_TYPE);
		virtual ~CSpirvShaderGenerator() = default;

		void Generate();

		void Write32(uint32);

		void WriteOp(spv::Op opcode)
		{
			m_outputStream.Write32((1 << 16) | static_cast<uint32>(opcode));
		}

		template <typename... ParamTypes>
		void WriteOp(spv::Op opcode, ParamTypes&&...params)
		{
			SpirvOpConverter::ConvertedParamArray convertedParams;
			convertedParams.reserve(sizeof...(params));
			SpirvOpConverter::ConvertParams(convertedParams, std::forward<ParamTypes>(params)...);
			uint32 paramSize = static_cast<uint32>(convertedParams.size()) + 1;
			m_outputStream.Write32((paramSize << 16) | static_cast<uint32>(opcode));
			m_outputStream.Write(convertedParams.data(), convertedParams.size() * sizeof(uint32));
		}

		void AllocateInputPointerIds();
		void DecorateInputPointerIds();
		void DeclareInputPointerIds();

		void AllocateOutputPointerIds();
		void DecorateOutputPointerIds();
		void DeclareOutputPointerIds();
		uint32 GetOutputPointerId(const CShaderBuilder::SYMBOLREF&);

		void GatherConstantsFromTemps();
		void DeclareTemporaryValueIds();

		void AllocateUniformStructsIds();
		void DecorateUniformStructIds();
		void DeclareUniformStructIds();

		void AllocateTextureIds();
		void DecorateTextureIds();
		void DeclareTextureIds();

		void RegisterFloatConstant(float);
		void RegisterIntConstant(int32);
		void RegisterUintConstant(uint32);

		uint32 LoadFromSymbol(const CShaderBuilder::SYMBOLREF&);
		void StoreToSymbol(const CShaderBuilder::SYMBOLREF&, uint32);
		std::pair<uint32, uint32> GetStructAccessChainParams(const CShaderBuilder::SYMBOLREF&);

		uint32 ExtractFloat4X(uint32);
		uint32 GetResultType(CShaderBuilder::SYMBOL_TYPE) const;

		static uint32 MapSemanticToLocation(Nuanceur::SEMANTIC, uint32);

		uint32 AllocateId();

		void Add(const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&);
		void BitwiseOp(spv::Op, const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&);
		void Sub(const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&);
		void Clamp(const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&,
			const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&);
		void AtomicImageOp(spv::Op, const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&, 
			const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&);
		void Load(const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&);
		void Store(const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&);

		enum
		{
			EMPTY_ID = 0
		};

		struct STRUCTINFO
		{
			uint32 typeId = EMPTY_ID;
			uint32 pointerTypeId = EMPTY_ID;
			uint32 variableId = EMPTY_ID;
			std::vector<uint32> components;
			std::map<uint32, uint32> memberIndices;
			uint32 memberIndex = 0;
			uint32 currentOffset = 0;
			bool isBufferBlock = false;
		};

		uint32 m_glslStd450ExtInst = EMPTY_ID;

		//Type Ids
		uint32 m_boolTypeId = EMPTY_ID;
		uint32 m_floatTypeId = EMPTY_ID;
		uint32 m_float4TypeId = EMPTY_ID;
		uint32 m_matrix44TypeId = EMPTY_ID;

		uint32 m_intTypeId = EMPTY_ID;
		uint32 m_uintTypeId = EMPTY_ID;
		uint32 m_int2TypeId = EMPTY_ID;
		uint32 m_int3TypeId = EMPTY_ID;
		uint32 m_int4TypeId = EMPTY_ID;
		uint32 m_uint4TypeId = EMPTY_ID;

		uint32 m_uintArrayTypeId = EMPTY_ID;

		uint32 m_inputFloat4PointerTypeId = EMPTY_ID;
		uint32 m_outputFloat4PointerTypeId = EMPTY_ID;
		uint32 m_inputInt3PointerTypeId = EMPTY_ID;
		uint32 m_inputUint4PointerTypeId = EMPTY_ID;

		uint32 m_outputPerVertexVariableId = EMPTY_ID;

		uint32 m_pushFloat4PointerTypeId = EMPTY_ID;
		uint32 m_pushInt4PointerTypeId = EMPTY_ID;
		uint32 m_pushMatrix44PointerTypeId = EMPTY_ID;

		uint32 m_uniformInt4PointerTypeId = EMPTY_ID;
		uint32 m_uniformUintPtrId = EMPTY_ID;

		//Sampled Image
		uint32 m_sampledImage2DTypeId = EMPTY_ID;
		uint32 m_sampledImageSamplerTypeId = EMPTY_ID;
		uint32 m_sampledImageSamplerPointerTypeId = EMPTY_ID;

		//Storage Image
		uint32 m_storageImage2DTypeId = EMPTY_ID;
		uint32 m_storageImage2DPointerTypeId = EMPTY_ID;
		uint32 m_imageUintPtrId = EMPTY_ID;

		Framework::CStream& m_outputStream;
		const CShaderBuilder& m_shaderBuilder;
		SHADER_TYPE m_shaderType = SHADER_TYPE_VERTEX;

		bool m_hasTextures = false;
		std::map<uint32, STRUCTINFO> m_structInfos;
		std::map<uint32, uint32> m_inputPointerIds;
		std::map<uint32, uint32> m_outputPointerIds;
		std::map<uint32, uint32> m_temporaryValueIds;
		std::map<uint32, uint32> m_texturePointerIds;
		std::map<float, uint32> m_floatConstantIds;
		std::map<int32, uint32> m_intConstantIds;
		std::map<uint32, uint32> m_uintConstantIds;
		uint32 m_nextId = EMPTY_ID + 1;
		uint32 m_endLabelId = 0;
	};
}
