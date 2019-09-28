#include "nuanceur/builder/ShaderBuilder.h"

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
	assert(sym.type == SYMBOL_TYPE_FLOAT4);
	auto temporaryValueIterator = m_temporaryValues.find(sym.index);
	if(temporaryValueIterator != std::end(m_temporaryValues))
	{
		result = temporaryValueIterator->second;
	}
	return result;
}

CShaderBuilder::CIntVector4 CShaderBuilder::GetTemporaryValueInt(const SYMBOL& sym) const
{
	CIntVector4 result(0, 0, 0, 0);
	assert(sym.location == SYMBOL_LOCATION_TEMPORARY);
	assert((sym.type == SYMBOL_TYPE_INT4) || (sym.type == SYMBOL_TYPE_UINT4));
	auto temporaryValueIterator = m_temporaryValuesInt.find(sym.index);
	if(temporaryValueIterator != std::end(m_temporaryValuesInt))
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

CShaderBuilder::SYMBOL CShaderBuilder::CreateTemporaryBool()
{
	SYMBOL sym;
	sym.owner    = this;
	sym.index    = m_currentTempIndex++;
	sym.type     = SYMBOL_TYPE_BOOL;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateTemporaryUint()
{
	SYMBOL sym;
	sym.owner    = this;
	sym.index    = m_currentTempIndex++;
	sym.type     = SYMBOL_TYPE_UINT4;
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

CShaderBuilder::SYMBOL CShaderBuilder::CreateConstantInt(int32 v1, int32 v2, int32 v3, int32 v4)
{
	//TODO: Check if constant already exists

	SYMBOL sym;
	sym.owner    = this;
	sym.index    = m_currentTempIndex++;
	sym.type     = SYMBOL_TYPE_INT4;
	sym.location = SYMBOL_LOCATION_TEMPORARY;
	m_symbols.push_back(sym);

	auto tempValue = CIntVector4(v1, v2, v3, v4);
	m_temporaryValuesInt.insert(std::make_pair(sym.index, tempValue));

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

CShaderBuilder::SYMBOL CShaderBuilder::CreateImage2DUint(unsigned int unit)
{
	SYMBOL sym;
	sym.owner    = this;
	sym.index    = unit;
	sym.type     = SYMBOL_TYPE_IMAGE2DUINT;
	sym.location = SYMBOL_LOCATION_TEXTURE;
	m_symbols.push_back(sym);

	return sym;
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateOptionalInput(bool available, SEMANTIC semantic, unsigned int semanticIndex)
{
	return available ? CreateInput(semantic, semanticIndex) : SYMBOL();
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateOptionalOutput(bool available, SEMANTIC semantic, unsigned int semanticIndex)
{
	return available ? CreateOutput(semantic, semanticIndex) : SYMBOL();
}

CShaderBuilder::SYMBOL CShaderBuilder::CreateOptionalUniformMatrix(bool available, const std::string& name)
{
	return available ? CreateUniformMatrix(name) : SYMBOL();
}
