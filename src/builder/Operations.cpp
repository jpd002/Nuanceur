#include "nuanceur/builder/Operations.h"
#include "nuanceur/builder/SwizzleSelector4.h"

using namespace Nuanceur;

#define CHECK_ISOPERANDVALID(a) assert((a).symbol.type != CShaderBuilder::SYMBOL_TYPE_NULL)

#define GENERATE_VECTOR_BINARY_OP(StatementOp, Operator, VectorType) \
	VectorType##Rvalue Nuanceur::operator Operator(const VectorType##Value& lhs, const VectorType##Value& rhs) \
	{ \
		CHECK_ISOPERANDVALID(lhs); \
		CHECK_ISOPERANDVALID(rhs); \
		auto owner = GetCommonOwner(lhs.symbol, rhs.symbol); \
		auto temp = VectorType##Rvalue(owner->CreateTemporary()); \
		owner->InsertStatement( \
			CShaderBuilder::STATEMENT(CShaderBuilder::StatementOp, temp, lhs, rhs) \
		); \
		return temp; \
	}

static CShaderBuilder* GetCommonOwner(const CShaderBuilder::SYMBOL& symbol1, const CShaderBuilder::SYMBOL& symbol2)
{
	assert(symbol1.owner != nullptr);
	assert(symbol2.owner != nullptr);
	assert(symbol1.owner == symbol2.owner);
	return symbol1.owner;
}

GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_ADD, +, CFloat2)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_ADD, +, CFloat3)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_ADD, +, CFloat4)

GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_SUBSTRACT, -, CFloat4)

GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_MULTIPLY, *, CFloat)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_MULTIPLY, *, CFloat2)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_MULTIPLY, *, CFloat4)

GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_DIVIDE, /, CFloat)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_DIVIDE, /, CFloat2)

CFloat4Rvalue Nuanceur::operator *(const CMatrix44Value& lhs, const CFloat4Value& rhs)
{
	auto owner = GetCommonOwner(lhs.symbol, rhs.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_MULTIPLY, temp, lhs, rhs)
	);
	return temp;
}

CBoolRvalue Nuanceur::operator <(const CFloatValue& lhs, const CFloatValue& rhs)
{
	CHECK_ISOPERANDVALID(lhs);
	CHECK_ISOPERANDVALID(rhs);
	auto owner = GetCommonOwner(lhs.symbol, rhs.symbol);
	auto temp = CBoolRvalue(owner->CreateTemporaryBool());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_COMPARE_LT, temp, lhs, rhs)
	);
	return temp;
}

void CBoolLvalue::operator =(const CBoolRvalue& rhs)
{
	CHECK_ISOPERANDVALID(*this);
	CHECK_ISOPERANDVALID(rhs);
	auto owner = rhs.symbol.owner;
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ASSIGN, *this, rhs)
	);
}

void CFloatLvalue::operator =(const CFloatRvalue& rhs)
{
	CHECK_ISOPERANDVALID(*this);
	CHECK_ISOPERANDVALID(rhs);
	auto owner = rhs.symbol.owner;
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ASSIGN, *this, rhs)
	);
}

void CFloat2Lvalue::operator =(const CFloat2Rvalue& rhs)
{
	CHECK_ISOPERANDVALID(*this);
	CHECK_ISOPERANDVALID(rhs);
	auto owner = rhs.symbol.owner;
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ASSIGN, *this, rhs)
	);
}

void CFloat4Lvalue::operator =(const CFloat4Rvalue& rhs)
{
	CHECK_ISOPERANDVALID(*this);
	CHECK_ISOPERANDVALID(rhs);
	auto owner = rhs.symbol.owner;
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ASSIGN, *this, rhs)
	);
}

CFloat4Rvalue Nuanceur::Clamp(const CFloat4Value& value, const CFloat4Value& min, const CFloat4Value& max)
{
	CHECK_ISOPERANDVALID(value);
	CHECK_ISOPERANDVALID(min);
	CHECK_ISOPERANDVALID(max);
	auto owner = GetCommonOwner(value.symbol, min.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_CLAMP, temp, value, min, max)
	);
	return temp;
}

CFloatRvalue Nuanceur::NewFloat(CShaderBuilder& owner, float x)
{
	auto literal = owner.CreateConstant(x, 0, 0, 0);
	return CFloatRvalue(literal);
}

CFloat2Rvalue Nuanceur::NewFloat2(CShaderBuilder& owner, float x, float y)
{
	auto literal = owner.CreateConstant(x, y, 0, 0);
	return CFloat2Rvalue(literal);
}

CFloat2Rvalue Nuanceur::NewFloat2(const CFloatValue& x, const CFloatValue& y)
{
	auto owner = GetCommonOwner(x.symbol, y.symbol);
	auto temp = CFloat2Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR2, temp, x, y)
	);
	return temp;
}

CFloat3Rvalue Nuanceur::NewFloat3(CShaderBuilder& owner, float x, float y, float z)
{
	auto literal = owner.CreateConstant(x, y, z, 0);
	return CFloat3Rvalue(literal);
}

CFloat4Rvalue Nuanceur::NewFloat4(CShaderBuilder& owner, float x, float y, float z, float w)
{
	auto literal = owner.CreateConstant(x, y, z, w);
	return CFloat4Rvalue(literal);
}

CFloat4Rvalue Nuanceur::NewFloat4(const CFloatValue& x, const CFloat3Value& yzw)
{
	auto owner = GetCommonOwner(x.symbol, yzw.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR4, temp, x, yzw)
	);
	return temp;
}

CFloat4Rvalue Nuanceur::NewFloat4(const CFloat3Value& xyz, const CFloatValue& w)
{
	auto owner = GetCommonOwner(xyz.symbol, w.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR4, temp, xyz, w)
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

CFloat4Rvalue Nuanceur::Sample(const CTexture2DValue& texture, const CFloat2Value& coord)
{
	auto owner = GetCommonOwner(texture.symbol, coord.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_SAMPLE, temp, texture, coord)
	);
	return temp;
}

CFloatRvalue Nuanceur::Saturate(const CFloatValue& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CFloatRvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_SATURATE, temp, rhs)
	);
	return temp;
}

void Nuanceur::BeginIf(CShaderBuilder& owner, const CBoolValue& condition)
{
	owner.InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_IF_BEGIN, Nuanceur::CShaderBuilder::SYMBOLREF(), condition)
	);
}

void Nuanceur::EndIf(CShaderBuilder& owner)
{
	owner.InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_IF_END, Nuanceur::CShaderBuilder::SYMBOLREF(), Nuanceur::CShaderBuilder::SYMBOLREF())
	);
}
