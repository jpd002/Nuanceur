#include "nuanceur/builder/Operations.h"
#include "nuanceur/builder/SwizzleSelector4.h"

using namespace Nuanceur;

static CShaderBuilder* GetCommonOwner(const CShaderBuilder::SYMBOL& symbol1, const CShaderBuilder::SYMBOL& symbol2)
{
	assert(symbol1.owner != nullptr);
	assert(symbol2.owner != nullptr);
	assert(symbol1.owner == symbol2.owner);
	return symbol1.owner;
}

CFloat3Rvalue Nuanceur::operator +(const CFloat3Value& lhs, const CFloat3Value& rhs)
{
	auto owner = GetCommonOwner(lhs.symbol, rhs.symbol);
	auto temp = CFloat3Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ADD, temp, lhs, rhs)
		);
	return temp;
}

CFloat4Rvalue Nuanceur::operator +(const CFloat4Value& lhs, const CFloat4Value& rhs)
{
	auto owner = GetCommonOwner(lhs.symbol, rhs.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ADD, temp, lhs, rhs)
		);
	return temp;
}

CFloat4Rvalue Nuanceur::operator -(const CFloat4Value& lhs, const CFloat4Value& rhs)
{
	auto owner = GetCommonOwner(lhs.symbol, rhs.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_SUBSTRACT, temp, lhs, rhs)
		);
	return temp;
}

CFloat4Rvalue Nuanceur::operator *(const CMatrix44Value& lhs, const CFloat4Value& rhs)
{
	auto owner = GetCommonOwner(lhs.symbol, rhs.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_MULTIPLY, temp, lhs, rhs)
	);
	return temp;
}

void CFloat4Lvalue::operator =(const CFloat4Rvalue& rhs)
{
	auto owner = rhs.symbol.owner;
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ASSIGN, *this, rhs)
		);
}

CFloat4Rvalue Nuanceur::NewFloat4(CShaderBuilder& owner, float cstX, float cstY, float cstZ, float cstW)
{
	auto literal = owner.CreateConstant(cstX, cstY, cstZ, cstW);
	return CFloat4Rvalue(literal);
}

CFloat4Rvalue Nuanceur::NewFloat4(const CFloat3Value& rhs, float constant)
{
	auto owner = rhs.symbol.owner;
	auto literal = CFloat4Rvalue(owner->CreateConstant(constant, 0, 0, 0), SWIZZLE_X); //This should be CFloat1Value
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR4, temp, rhs, literal)
		);
	return temp;
}

CFloat4Rvalue Nuanceur::Normalize(const CFloat4Value& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NORMALIZE, temp, rhs)
		);
	return temp;
}

CFloat4Rvalue Nuanceur::Sample(const CTexture2DValue& texture, const CFloat4Value& coord)
{
	auto owner = GetCommonOwner(texture.symbol, coord.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_SAMPLE, temp, texture, coord)
	);
	return temp;

}
