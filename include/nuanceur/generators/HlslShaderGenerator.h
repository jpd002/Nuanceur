#pragma once

#include "nuanceur/builder/ShaderBuilder.h"

namespace Nuanceur
{
	class CHlslShaderGenerator
	{
	public:
		static std::string		Generate(const std::string&, const CShaderBuilder&);

	private:
								CHlslShaderGenerator(const CShaderBuilder&);

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
	};
}
