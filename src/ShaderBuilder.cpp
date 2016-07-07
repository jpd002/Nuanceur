#include "ShaderBuilder.h"

using namespace Nuanceur;

const CShaderBuilder::SymbolArray& CShaderBuilder::GetSymbols() const
{
	return m_symbols;
}

CShaderBuilder::SEMANTIC_INFO CShaderBuilder::GetInputSemantic(const SYMBOL& sym) const
{
	assert(sym.location == SYMBOL_LOCATION_INPUT);
	return m_inputSemantics.find(sym.index)->second;
}

CShaderBuilder::SEMANTIC_INFO CShaderBuilder::GetOutputSemantic(const SYMBOL& sym) const
{
	assert(sym.location == SYMBOL_LOCATION_OUTPUT);
	return m_outputSemantics.find(sym.index)->second;
}

std::string CShaderBuilder::GetUniformName(const SYMBOL& sym) const
{
	assert(sym.location == SYMBOL_LOCATION_UNIFORM);
	return m_uniformNames.find(sym.index)->second;
}

CVector4 CShaderBuilder::GetTemporaryValue(const SYMBOL& sym) const
{
	CVector4 result(0, 0, 0, 0);
	assert(sym.location == SYMBOL_LOCATION_TEMPORARY);
	auto temporaryValueIterator = m_temporaryValues.find(sym.index);
	if(temporaryValueIterator != std::end(m_temporaryValues))
	{
		result = temporaryValueIterator->second;
	}
	return result;
}

const CShaderBuilder::StatementList& CShaderBuilder::GetStatements() const
{
	return m_statements;
}

void CShaderBuilder::InsertStatement(const STATEMENT& statement)
{
	m_statements.push_back(statement);
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateInput(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.owner    = this;
	sym.index    = m_currentInputIndex++;
	sym.type     = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_INPUT;
	m_symbols.push_back(sym);

	m_inputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateOutput(SEMANTIC semantic, unsigned int semanticIndex)
{
	SYMBOL sym;
	sym.owner    = this;
	sym.index    = m_currentOutputIndex++;
	sym.type     = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_OUTPUT;
	m_symbols.push_back(sym);

	m_outputSemantics.insert(std::make_pair(sym.index, SEMANTIC_INFO(semantic, semanticIndex)));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateTemporary()
{
	SYMBOL sym;
	sym.owner    = this;
	sym.index    = m_currentTempIndex++;
	sym.type     = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateConstant(float v1, float v2, float v3, float v4)
{
	//TODO: Check if constant already exists

	SYMBOL sym;
	sym.owner    = this;
	sym.index    = m_currentTempIndex++;
	sym.type     = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	auto tempValue = CVector4(v1, v2, v3, v4);
	m_temporaryValues.insert(std::make_pair(sym.index, tempValue));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateUniformFloat4(const std::string& name)
{
	SYMBOL sym;
	sym.owner    = this;
	sym.index    = m_currentTempIndex++;
	sym.type     = SYMBOL_TYPE_FLOAT4;
	sym.location = SYMBOL_LOCATION_UNIFORM;
	m_symbols.push_back(sym);

	m_uniformNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateUniformMatrix(const std::string& name)
{
	SYMBOL sym;
	sym.owner    = this;
	sym.index    = m_currentTempIndex++;
	sym.type     = SYMBOL_TYPE_MATRIX;
	sym.location = SYMBOL_LOCATION_UNIFORM;
	m_symbols.push_back(sym);

	m_uniformNames.insert(std::make_pair(sym.index, name));

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateTexture2D(unsigned int unit)
{
	SYMBOL sym;
	sym.owner    = this;
	sym.index    = unit;
	sym.type     = SYMBOL_TYPE_TEXTURE2D;
	sym.location = SYMBOL_LOCATION_TEXTURE;
	m_symbols.push_back(sym);

	return sym;
}
