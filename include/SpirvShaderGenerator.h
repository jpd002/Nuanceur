#pragma once

#include <map>
#include "ShaderBuilder.h"
#include "Stream.h"
#include <vulkan/spirv.hpp>

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
			SHADER_TYPE_FRAGMENT
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

		void DeclareTemporaryValueIds();

		void DecorateUniformStructIds();
		void DeclareUniformStructIds();

		void AllocateTextureIds();
		void DecorateTextureIds();
		void DeclareTextureIds();

		uint32 LoadFromSymbol(const CShaderBuilder::SYMBOLREF&);
		void StoreToSymbol(const CShaderBuilder::SYMBOLREF&, uint32);

		static uint32 MapSemanticToLocation(Nuanceur::SEMANTIC, uint32);

		uint32 AllocateId();

		//Type Ids
		uint32 m_floatTypeId = 0;
		uint32 m_float4TypeId = 0;
		uint32 m_inputFloat4PointerTypeId = 0;

		uint32 m_outputFloat4PointerTypeId = 0;

		uint32 m_outputPerVertexVariableId = 0;

		uint32 m_uniformStructTypeId = 0;
		uint32 m_pushUniformStructPointerTypeId = 0;
		uint32 m_pushFloat4PointerTypeId = 0;
		uint32 m_pushUniformVariableId = 0;

		uint32 m_image2DTypeId = 0;
		uint32 m_imageSamplerTypeId = 0;
		uint32 m_imageSamplerPointerTypeId = 0;

		uint32 m_int32ZeroConstantId = 0;

		Framework::CStream& m_outputStream;
		const CShaderBuilder& m_shaderBuilder;
		SHADER_TYPE m_shaderType = SHADER_TYPE_VERTEX;

		bool m_hasUniforms = false;
		bool m_hasTextures = false;
		std::map<uint32, uint32> m_inputPointerIds;
		std::map<uint32, uint32> m_outputPointerIds;
		std::map<uint32, uint32> m_temporaryValueIds;
		std::map<uint32, uint32> m_uniformStructMemberIds;
		std::map<uint32, uint32> m_texturePointerIds;
		uint32 m_nextId = 1;
	};
}
