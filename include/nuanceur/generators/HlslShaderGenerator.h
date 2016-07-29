#pragma once

#include "Types.h"
#include "nuanceur/builder/ShaderBuilder.h"

namespace Nuanceur
{
	class CHlslShaderGenerator
	{
	public:
		enum FLAGS : uint32
		{
			FLAG_COMBINED_SAMPLER_TEXTURE = 0x01,    //Use tex2D/texCUBE instead of texture.Sample
		};

		static std::string		Generate(const std::string&, const CShaderBuilder&, uint32 flags = 0);

	private:
								CHlslShaderGenerator(const CShaderBuilder&, uint32);

		std::string				Generate(const std::string&) const;

		std::string				GenerateInputStruct() const;
		std::string				GenerateOutputStruct() const;
		std::string				GenerateConstants() const;
		std::string				GenerateSamplers() const;

		std::string				MakeSymbolName(const CShaderBuilder::SYMBOL&) const;
		std::string				MakeLocalSymbolName(const CShaderBuilder::SYMBOL&) const;
		static std::string		MakeSemanticName(CShaderBuilder::SEMANTIC_INFO);
		static std::string		MakeTypeName(CShaderBuilder::SYMBOL_TYPE);
		std::string				PrintSymbolRef(const CShaderBuilder::SYMBOLREF&) const;

		const CShaderBuilder&	m_shaderBuilder;
		uint32					m_flags = 0;
	};
}
