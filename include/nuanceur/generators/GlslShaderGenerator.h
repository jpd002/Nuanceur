#pragma once

#include "nuanceur/builder/ShaderBuilder.h"
#include <array>

namespace Nuanceur
{
	class CGlslShaderGenerator
	{
	public:
		enum SHADER_TYPE
		{
			SHADER_TYPE_VERTEX,
			SHADER_TYPE_FRAGMENT
		};

		static std::string Generate(const CShaderBuilder&, SHADER_TYPE, uint32 = 0);

	private:
		CGlslShaderGenerator(const CShaderBuilder&, SHADER_TYPE, uint32 = 0);

		std::string Generate() const;

		std::string GenerateInputs() const;
		std::string GenerateOutputs() const;
		std::string GenerateUniforms() const;
		std::string GenerateSamplers() const;

		std::string MakeSymbolName(const CShaderBuilder::SYMBOL&) const;
		std::string MakeLocalSymbolName(const CShaderBuilder::SYMBOL&) const;
		static std::string MakeSemanticName(CShaderBuilder::SEMANTIC_INFO);
		static std::string MakeTypeName(CShaderBuilder::SYMBOL_TYPE);
		std::string PrintSymbolRef(const CShaderBuilder::SYMBOLREF&) const;

		std::string EmitConversion(const std::array<const char*, 4>&, const CShaderBuilder::SYMBOLREF&, const CShaderBuilder::SYMBOLREF&) const;

		const CShaderBuilder& m_shaderBuilder;
		SHADER_TYPE m_shaderType = SHADER_TYPE_VERTEX;
		uint32 m_glslVersion = 0;
	};
}
