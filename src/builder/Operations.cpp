#include "nuanceur/builder/Operations.h"
#include "nuanceur/builder/FloatSwizzleSelector.h"
#include "nuanceur/builder/FloatSwizzleSelector4.h"
#include "nuanceur/builder/IntSwizzleSelector4.h"
#include "nuanceur/builder/UintSwizzleSelector4.h"

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

#define GENERATE_VECTOR_BINARY_INT_OP(StatementOp, Operator, VectorType) \
	VectorType##Rvalue Nuanceur::operator Operator(const VectorType##Value& lhs, const VectorType##Value& rhs) \
	{ \
		CHECK_ISOPERANDVALID(lhs); \
		CHECK_ISOPERANDVALID(rhs); \
		auto owner = GetCommonOwner(lhs.symbol, rhs.symbol); \
		auto temp = VectorType##Rvalue(owner->CreateTemporaryInt()); \
		owner->InsertStatement( \
			CShaderBuilder::STATEMENT(CShaderBuilder::StatementOp, temp, lhs, rhs) \
		); \
		return temp; \
	}

#define GENERATE_VECTOR_BINARY_UINT_OP(StatementOp, Operator, VectorType) \
	VectorType##Rvalue Nuanceur::operator Operator(const VectorType##Value& lhs, const VectorType##Value& rhs) \
	{ \
		CHECK_ISOPERANDVALID(lhs); \
		CHECK_ISOPERANDVALID(rhs); \
		auto owner = GetCommonOwner(lhs.symbol, rhs.symbol); \
		auto temp = VectorType##Rvalue(owner->CreateTemporaryUint()); \
		owner->InsertStatement( \
			CShaderBuilder::STATEMENT(CShaderBuilder::StatementOp, temp, lhs, rhs) \
		); \
		return temp; \
	}

#define GENERATE_VECTOR_COMPARE_OP(StatementOp, Operator, VectorType) \
	CBoolRvalue Nuanceur::operator Operator(const VectorType##Value& lhs, const VectorType##Value& rhs) \
	{ \
		CHECK_ISOPERANDVALID(lhs); \
		CHECK_ISOPERANDVALID(rhs); \
		auto owner = GetCommonOwner(lhs.symbol, rhs.symbol); \
		auto temp = CBoolRvalue(owner->CreateTemporaryBool()); \
		owner->InsertStatement( \
			CShaderBuilder::STATEMENT(CShaderBuilder::StatementOp, temp, lhs, rhs) \
		); \
		return temp; \
	}

#define GENERATE_VECTOR_ASSIGN(VectorType) \
	void VectorType##Lvalue::operator =(const VectorType##Rvalue& rhs) \
	{ \
		CHECK_ISOPERANDVALID(*this); \
		CHECK_ISOPERANDVALID(rhs); \
		auto owner = rhs.symbol.owner; \
		owner->InsertStatement( \
			CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ASSIGN, *this, rhs) \
		); \
	}

static CShaderBuilder* GetCommonOwner(const CShaderBuilder::SYMBOL& symbol1, const CShaderBuilder::SYMBOL& symbol2)
{
	assert(symbol1.owner != nullptr);
	assert(symbol2.owner != nullptr);
	assert(symbol1.owner == symbol2.owner);
	return symbol1.owner;
}

GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_ADD, +, CFloat)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_ADD, +, CFloat2)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_ADD, +, CFloat3)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_ADD, +, CFloat4)
GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_ADD, +, CInt)
GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_ADD, +, CInt2)
GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_ADD, +, CInt3)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_ADD, +, CUint)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_ADD, +, CUint3)

GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_SUBSTRACT, -, CFloat)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_SUBSTRACT, -, CFloat3)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_SUBSTRACT, -, CFloat4)
GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_SUBSTRACT, -, CInt)
GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_SUBSTRACT, -, CInt3)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_SUBSTRACT, -, CUint3)

GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_MULTIPLY, *, CFloat)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_MULTIPLY, *, CFloat2)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_MULTIPLY, *, CFloat3)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_MULTIPLY, *, CFloat4)
GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_MULTIPLY, *, CInt)
GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_MULTIPLY, *, CInt2)
GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_MULTIPLY, *, CInt3)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_MULTIPLY, *, CUint)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_MULTIPLY, *, CUint3)

GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_DIVIDE, /, CFloat)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_DIVIDE, /, CFloat2)
GENERATE_VECTOR_BINARY_OP(STATEMENT_OP_DIVIDE, /, CFloat4)
GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_DIVIDE, /, CInt)

GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_MODULO, %, CInt)

GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_LSHIFT, <<, CInt)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_LSHIFT, <<, CUint)

GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_RSHIFT, >>, CInt)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_RSHIFT, >>, CUint)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_RSHIFT, >>, CUint3)

GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_AND, &, CInt)
GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_AND, &, CInt4)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_AND, &, CUint)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_AND, &, CUint4)

GENERATE_VECTOR_BINARY_INT_OP(STATEMENT_OP_OR, |, CInt)
GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_OR, |, CUint)

GENERATE_VECTOR_BINARY_UINT_OP(STATEMENT_OP_XOR, ^, CUint)

GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_EQ, ==, CFloat)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_LT, <, CFloat)

GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_EQ, ==, CInt)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_NE, !=, CInt)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_LT, <, CInt)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_LE, <=, CInt)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_GT, >, CInt)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_GE, >=, CInt)

GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_EQ, ==, CUint)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_NE, !=, CUint)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_LT, <, CUint)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_LE, <=, CUint)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_GT, >, CUint)
GENERATE_VECTOR_COMPARE_OP(STATEMENT_OP_COMPARE_GE, >=, CUint)

GENERATE_VECTOR_ASSIGN(CBool)
GENERATE_VECTOR_ASSIGN(CFloat)
GENERATE_VECTOR_ASSIGN(CFloat2)
GENERATE_VECTOR_ASSIGN(CFloat4)
GENERATE_VECTOR_ASSIGN(CInt)
GENERATE_VECTOR_ASSIGN(CInt2)
GENERATE_VECTOR_ASSIGN(CInt4)
GENERATE_VECTOR_ASSIGN(CUint)
GENERATE_VECTOR_ASSIGN(CUint4)

CFloat4Rvalue Nuanceur::operator *(const CMatrix44Value& lhs, const CFloat4Value& rhs)
{
	auto owner = GetCommonOwner(lhs.symbol, rhs.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_MULTIPLY, temp, lhs, rhs)
	);
	return temp;
}

CUintRvalue Nuanceur::operator ~(const CUintValue& lhs)
{
	CHECK_ISOPERANDVALID(lhs);
	auto owner = lhs.symbol.owner;
	auto temp = CUintRvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NOT, temp, lhs)
	);
	return temp;
}

CBoolRvalue Nuanceur::operator &&(const CBoolValue& lhs, const CBoolValue& rhs)
{
	CHECK_ISOPERANDVALID(lhs);
	CHECK_ISOPERANDVALID(rhs);
	auto owner = GetCommonOwner(lhs.symbol, rhs.symbol);
	auto temp = CBoolRvalue(owner->CreateTemporaryBool());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_LOGICAL_AND, temp, lhs, rhs)
	);
	return temp;
}

CBoolRvalue Nuanceur::operator ||(const CBoolValue& lhs, const CBoolValue& rhs)
{
	CHECK_ISOPERANDVALID(lhs);
	CHECK_ISOPERANDVALID(rhs);
	auto owner = GetCommonOwner(lhs.symbol, rhs.symbol);
	auto temp = CBoolRvalue(owner->CreateTemporaryBool());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_LOGICAL_OR, temp, lhs, rhs)
	);
	return temp;
}

CBoolRvalue Nuanceur::operator !(const CBoolValue& lhs)
{
	CHECK_ISOPERANDVALID(lhs);
	auto owner = lhs.symbol.owner;
	auto temp = CBoolRvalue(owner->CreateTemporaryBool());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_LOGICAL_NOT, temp, lhs)
	);
	return temp;
}

CIntRvalue Nuanceur::Clamp(const CIntValue& value, const CIntValue& min, const CIntValue& max)
{
	CHECK_ISOPERANDVALID(value);
	CHECK_ISOPERANDVALID(min);
	CHECK_ISOPERANDVALID(max);
	auto owner = GetCommonOwner(value.symbol, min.symbol);
	auto temp = CIntRvalue(owner->CreateTemporaryInt());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_CLAMP, temp, value, min, max)
	);
	return temp;
}

CInt4Rvalue Nuanceur::Clamp(const CInt4Value& value, const CInt4Value& min, const CInt4Value& max)
{
	CHECK_ISOPERANDVALID(value);
	CHECK_ISOPERANDVALID(min);
	CHECK_ISOPERANDVALID(max);
	auto owner = GetCommonOwner(value.symbol, min.symbol);
	auto temp = CInt4Rvalue(owner->CreateTemporaryInt());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_CLAMP, temp, value, min, max)
	);
	return temp;
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

CFloat2Rvalue Nuanceur::Fract(const CFloat2Value& value)
{
	CHECK_ISOPERANDVALID(value);
	auto owner = value.symbol.owner;
	auto temp = CFloat2Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_FRACT, temp, value));
	return temp;
}

CUint4Rvalue Nuanceur::Min(const CUint4Value& x, const CUint4Value& y)
{
	CHECK_ISOPERANDVALID(x);
	CHECK_ISOPERANDVALID(y);
	auto owner = GetCommonOwner(x.symbol, y.symbol);
	auto temp = CUint4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_MIN, temp, x, y)
	);
	return temp;
}

CFloatRvalue Nuanceur::Mix(const CFloatValue& x, const CFloatValue& y, const CFloatValue& a)
{
	CHECK_ISOPERANDVALID(x);
	CHECK_ISOPERANDVALID(y);
	CHECK_ISOPERANDVALID(a);
	auto owner = GetCommonOwner(x.symbol, y.symbol);
	auto temp = CFloatRvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_MIX, temp, x, y, a)
	);
	return temp;
}

CFloat3Rvalue Nuanceur::Mix(const CFloat3Value& x, const CFloat3Value& y, const CFloat3Value& a)
{
	CHECK_ISOPERANDVALID(x);
	CHECK_ISOPERANDVALID(y);
	CHECK_ISOPERANDVALID(a);
	auto owner = GetCommonOwner(x.symbol, y.symbol);
	auto temp = CFloat3Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_MIX, temp, x, y, a)
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

CFloatRvalue Nuanceur::Saturate(const CFloatValue& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CFloatRvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_SATURATE, temp, rhs)
	);
	return temp;
}

CInt3Rvalue Nuanceur::ShiftRightArithmetic(const CInt3Value& x, const CInt3Value& y)
{
	CHECK_ISOPERANDVALID(x);
	CHECK_ISOPERANDVALID(y);
	auto owner = GetCommonOwner(x.symbol, y.symbol);
	auto temp = CInt3Rvalue(owner->CreateTemporaryInt());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_RSHIFT_ARITHMETIC, temp, x, y)
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

CFloat4Rvalue Nuanceur::NewFloat4(const CFloatValue& x, const CFloatValue& y, const CFloatValue& z, const CFloatValue& w)
{
	auto owner = GetCommonOwner(x.symbol, y.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR4, temp, x, y, z, w)
	);
	return temp;
}

CFloat4Rvalue Nuanceur::NewFloat4(const CFloat2Value& xy, const CFloat2Value& zw)
{
	auto owner = GetCommonOwner(xy.symbol, zw.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR4, temp, xy, zw)
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

CIntRvalue Nuanceur::NewInt(CShaderBuilder& owner, int32 x)
{
	auto literal = owner.CreateConstantInt(x, 1, 1, 1);
	return CIntRvalue(literal);
}

CInt2Rvalue Nuanceur::NewInt2(CShaderBuilder& owner, int32 x, int32 y)
{
	auto literal = owner.CreateConstantInt(x, y, 1, 1);
	return CInt2Rvalue(literal);
}

CInt2Rvalue Nuanceur::NewInt2(const CIntValue& x, const CIntValue& y)
{
	auto owner = GetCommonOwner(x.symbol, y.symbol);
	auto temp = CInt2Rvalue(owner->CreateTemporaryInt());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR2, temp, x, y)
	);
	return temp;
}

CInt3Rvalue Nuanceur::NewInt3(CShaderBuilder& owner, int32 x, int32 y, int32 z)
{
	auto literal = owner.CreateConstantInt(x, y, z, 1);
	return CInt3Rvalue(literal);
}

CInt4Rvalue Nuanceur::NewInt4(CShaderBuilder& owner, int32 x, int32 y, int32 z, int32 w)
{
	auto literal = owner.CreateConstantInt(x, y, z, w);
	return CInt4Rvalue(literal);
}

CInt4Rvalue Nuanceur::NewInt4(const CIntValue& x, const CIntValue& y, const CIntValue& z, const CIntValue& w)
{
	auto owner = GetCommonOwner(x.symbol, y.symbol);
	auto temp = CInt4Rvalue(owner->CreateTemporaryInt());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR4, temp, x, y, z, w)
	);
	return temp;
}

CInt4Rvalue Nuanceur::NewInt4(const CInt3Value& xyz, const CIntValue& w)
{
	auto owner = GetCommonOwner(xyz.symbol, w.symbol);
	auto temp = CInt4Rvalue(owner->CreateTemporaryInt());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR4, temp, xyz, w)
	);
	return temp;
}

CUintRvalue Nuanceur::NewUint(CShaderBuilder& owner, uint32 x)
{
	auto literal = owner.CreateConstantUint(x, 1, 1, 1);
	return CUintRvalue(literal);
}

CUint3Rvalue Nuanceur::NewUint3(CShaderBuilder& owner, uint32 x, uint32 y, uint32 z)
{
	auto literal = owner.CreateConstantUint(x, y, z, 1);
	return CUint3Rvalue(literal);
}

CUint4Rvalue Nuanceur::NewUint4(CShaderBuilder& owner, uint32 x, uint32 y, uint32 z, uint32 w)
{
	auto literal = owner.CreateConstantUint(x, y, z, w);
	return CUint4Rvalue(literal);
}

CUint4Rvalue Nuanceur::NewUint4(const CUintValue& x, const CUint3Value& yzw)
{
	auto owner = GetCommonOwner(x.symbol, yzw.symbol);
	auto temp = CUint4Rvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR4, temp, x, yzw)
	);
	return temp;
}

CUint4Rvalue Nuanceur::NewUint4(const CUint3Value& xyz, const CUintValue& w)
{
	auto owner = GetCommonOwner(xyz.symbol, w.symbol);
	auto temp = CUint4Rvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_NEWVECTOR4, temp, xyz, w)
	);
	return temp;
}

CBoolRvalue Nuanceur::NewBool(CShaderBuilder& owner, bool x)
{
	auto literal = owner.CreateConstantBool(x);
	return CBoolRvalue(literal);
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

CUint4Rvalue Nuanceur::Load(const CImageUint2DValue& image, const CInt2Value& coord)
{
	auto owner = GetCommonOwner(image.symbol, coord.symbol);
	auto temp = CUint4Rvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_LOAD, temp, image, coord)
	);
	return temp;
}

void Nuanceur::Store(const CImageUint2DValue& image, const CInt2Value& coord, const CUint4Value& value)
{
	auto owner = GetCommonOwner(image.symbol, coord.symbol);
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_STORE, CShaderBuilder::SYMBOLREF(), image, coord, value)
	);
}

CUintRvalue Nuanceur::AtomicAnd(const CImageUint2DValue& image, const CInt2Value& coord, const CUintValue& value)
{
	auto owner = GetCommonOwner(image.symbol, coord.symbol);
	auto temp = CUintRvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ATOMICAND, temp, image, coord, value)
	);
	return temp;
}

CUintRvalue Nuanceur::AtomicOr(const CImageUint2DValue& image, const CInt2Value& coord, const CUintValue& value)
{
	auto owner = GetCommonOwner(image.symbol, coord.symbol);
	auto temp = CUintRvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ATOMICOR, temp, image, coord, value)
	);
	return temp;
}

CUintRvalue Nuanceur::Load(const CArrayUintValue& buffer, const CIntValue& index)
{
	auto owner = GetCommonOwner(buffer.symbol, index.symbol);
	auto temp = CUintRvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_LOAD, temp, buffer, index)
	);
	return temp;
}

void Nuanceur::Store(const CArrayUintValue& buffer, const CIntValue& index, const CUintValue& value)
{
	auto owner = GetCommonOwner(buffer.symbol, index.symbol);
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_STORE, CShaderBuilder::SYMBOLREF(), buffer, index, value)
	);
}

void Nuanceur::Store(const CArrayUshortValue& buffer, const CIntValue& index, const CUshortValue& value)
{
	auto owner = GetCommonOwner(buffer.symbol, index.symbol);
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_STORE16, CShaderBuilder::SYMBOLREF(), buffer, index, value)
	);
}

void Nuanceur::Store(const CArrayUcharValue& buffer, const CIntValue& index, const CUcharValue& value)
{
	auto owner = GetCommonOwner(buffer.symbol, index.symbol);
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_STORE8, CShaderBuilder::SYMBOLREF(), buffer, index, value)
	);
}

CUintRvalue Nuanceur::AtomicAnd(const CArrayUintValue& buffer, const CIntValue& index, const CUintValue& value)
{
	auto owner = GetCommonOwner(buffer.symbol, index.symbol);
	auto temp = CUintRvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ATOMICAND, temp, buffer, index, value)
	);
	return temp;
}

CUintRvalue Nuanceur::AtomicOr(const CArrayUintValue& buffer, const CIntValue& index, const CUintValue& value)
{
	auto owner = GetCommonOwner(buffer.symbol, index.symbol);
	auto temp = CUintRvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_ATOMICOR, temp, buffer, index, value)
	);
	return temp;
}

CFloat4Rvalue Nuanceur::Load(const CSubpassInputValue& image, const CInt2Value& coord)
{
	auto owner = GetCommonOwner(image.symbol, coord.symbol);
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_LOAD, temp, image, coord)
	);
	return temp;
}

CUint4Rvalue Nuanceur::Load(const CSubpassInputUintValue& image, const CInt2Value& coord)
{
	auto owner = GetCommonOwner(image.symbol, coord.symbol);
	auto temp = CUint4Rvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_LOAD, temp, image, coord)
	);
	return temp;
}

CFloatRvalue Nuanceur::ToFloat(const CUintValue& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CFloatRvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOFLOAT, temp, rhs)
	);
	return temp;
}

CFloatRvalue Nuanceur::ToFloat(const CIntValue& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CFloatRvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOFLOAT, temp, rhs)
	);
	return temp;
}

CFloat2Rvalue Nuanceur::ToFloat(const CInt2Value& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CFloat2Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOFLOAT, temp, rhs)
	);
	return temp;
}

CFloat4Rvalue Nuanceur::ToFloat(const CInt4Value& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOFLOAT, temp, rhs)
	);
	return temp;
}

CFloat4Rvalue Nuanceur::ToFloat(const CUint4Value& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CFloat4Rvalue(owner->CreateTemporary());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOFLOAT, temp, rhs)
	);
	return temp;
}

CIntRvalue Nuanceur::ToInt(const CUintValue& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CIntRvalue(owner->CreateTemporaryInt());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOINT, temp, rhs)
	);
	return temp;
}

CInt2Rvalue Nuanceur::ToInt(const CFloat2Value& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CInt2Rvalue(owner->CreateTemporaryInt());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOINT, temp, rhs)
	);
	return temp;
}

CInt4Rvalue Nuanceur::ToInt(const CFloat4Value& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CInt4Rvalue(owner->CreateTemporaryInt());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOINT, temp, rhs)
	);
	return temp;
}

CUintRvalue Nuanceur::ToUint(const CFloatValue& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CUintRvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOUINT, temp, rhs)
	);
	return temp;
}

CUintRvalue Nuanceur::ToUint(const CIntValue& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CUintRvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOUINT, temp, rhs)
	);
	return temp;
}

CUshortRvalue Nuanceur::ToUshort(const CUintValue& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CUshortRvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOUSHORT, temp, rhs)
	);
	return temp;
}

CUcharRvalue Nuanceur::ToUchar(const CUintValue& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CUcharRvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOUCHAR, temp, rhs)
	);
	return temp;
}

CUint4Rvalue Nuanceur::ToUint(const CFloat4Value& rhs)
{
	auto owner = rhs.symbol.owner;
	auto temp = CUint4Rvalue(owner->CreateTemporaryUint());
	owner->InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_TOUINT, temp, rhs)
	);
	return temp;
}

void Nuanceur::Return(CShaderBuilder& owner)
{
	owner.InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_RETURN, CShaderBuilder::SYMBOLREF(), CShaderBuilder::SYMBOLREF())
	);
}

void Nuanceur::BeginInvocationInterlock(CShaderBuilder& owner)
{
	owner.InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_INVOCATION_INTERLOCK_BEGIN, CShaderBuilder::SYMBOLREF(), CShaderBuilder::SYMBOLREF())
	);
}

void Nuanceur::EndInvocationInterlock(CShaderBuilder& owner)
{
	owner.InsertStatement(
		CShaderBuilder::STATEMENT(CShaderBuilder::STATEMENT_OP_INVOCATION_INTERLOCK_END, CShaderBuilder::SYMBOLREF(), CShaderBuilder::SYMBOLREF())
	);
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
