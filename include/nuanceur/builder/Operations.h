#pragma once

#include "Types.h"
#include "ShaderBuilder.h"

#include "ArrayUintValue.h"
#include "BoolValue.h"
#include "FloatValue.h"
#include "Float2Value.h"
#include "Float3Value.h"
#include "Float4Value.h"
#include "ImageUint2DValue.h"
#include "IntValue.h"
#include "Int2Value.h"
#include "Int4Value.h"
#include "Matrix44Value.h"
#include "Texture2DValue.h"
#include "UintValue.h"
#include "Uint3Value.h"
#include "Uint4Value.h"

namespace Nuanceur
{
	CFloatRvalue operator +(const CFloatValue& lhs, const CFloatValue& rhs);
	CFloat2Rvalue operator +(const CFloat2Value& lhs, const CFloat2Value& rhs);
	CFloat3Rvalue operator +(const CFloat3Value& lhs, const CFloat3Value& rhs);
	CFloat4Rvalue operator +(const CFloat4Value& lhs, const CFloat4Value& rhs);
	CIntRvalue operator +(const CIntValue& lhs, const CIntValue& rhs);
	CUintRvalue operator +(const CUintValue& lhs, const CUintValue& rhs);

	CFloat3Rvalue operator -(const CFloat3Value& lhs, const CFloat3Value& rhs);
	CFloat4Rvalue operator -(const CFloat4Value& lhs, const CFloat4Value& rhs);
	CIntRvalue operator -(const CIntValue& lhs, const CIntValue& rhs);

	CFloatRvalue operator *(const CFloatValue& lhs, const CFloatValue& rhs);
	CFloat2Rvalue operator *(const CFloat2Value& lhs, const CFloat2Value& rhs);
	CFloat3Rvalue operator *(const CFloat3Value& lhs, const CFloat3Value& rhs);
	CFloat4Rvalue operator *(const CFloat4Value& lhs, const CFloat4Value& rhs);

	CIntRvalue operator *(const CIntValue& lhs, const CIntValue& rhs);
	CInt2Rvalue operator *(const CInt2Value& lhs, const CInt2Value& rhs);

	CUintRvalue operator *(const CUintValue& lhs, const CUintValue& rhs);

	CFloatRvalue operator /(const CFloatValue& lhs, const CFloatValue& rhs);
	CFloat2Rvalue operator /(const CFloat2Value& lhs, const CFloat2Value& rhs);
	CFloat4Rvalue operator /(const CFloat4Value& lhs, const CFloat4Value& rhs);
	CIntRvalue operator /(const CIntValue& lhs, const CIntValue& rhs);

	CIntRvalue operator %(const CIntValue& lhs, const CIntValue& rhs);

	CIntRvalue operator >>(const CIntValue& lhs, const CIntValue& rhs);
	CUintRvalue operator >>(const CUintValue& lhs, const CUintValue& rhs);

	CIntRvalue operator <<(const CIntValue& lhs, const CIntValue& rhs);
	CUintRvalue operator <<(const CUintValue& lhs, const CUintValue& rhs);

	CIntRvalue operator &(const CIntValue& lhs, const CIntValue& rhs);
	CUintRvalue operator &(const CUintValue& lhs, const CUintValue& rhs);

	CIntRvalue operator |(const CIntValue& lhs, const CIntValue& rhs);
	CUintRvalue operator |(const CUintValue& lhs, const CUintValue& rhs);

	CUintRvalue operator ^(const CUintValue& lhs, const CUintValue& rhs);

	CFloat4Rvalue operator *(const CMatrix44Value& lhs, const CFloat4Value& rhs);

	CUintRvalue operator ~(const CUintValue& lhs);

	CBoolRvalue operator &&(const CBoolValue& lhs, const CBoolValue& rhs);
	CBoolRvalue operator ||(const CBoolValue& lhs, const CBoolValue& rhs);

	CBoolRvalue operator !(const CBoolValue& lhs);

	CBoolRvalue operator ==(const CFloatValue& lhs, const CFloatValue& rhs);
	CBoolRvalue operator <(const CFloatValue& lhs, const CFloatValue& rhs);

	CBoolRvalue operator >=(const CIntValue& lhs, const CIntValue& rhs);

	CBoolRvalue operator ==(const CUintValue& lhs, const CUintValue& rhs);
	CBoolRvalue operator >(const CUintValue& lhs, const CUintValue& rhs);
	CBoolRvalue operator >=(const CUintValue& lhs, const CUintValue& rhs);

	CFloat4Rvalue Clamp(const CFloat4Value&, const CFloat4Value&, const CFloat4Value&);
	CIntRvalue Clamp(const CIntValue&, const CIntValue&, const CIntValue&);
	CFloatRvalue Mix(const CFloatValue&, const CFloatValue&, const CFloatValue&);
	CFloat4Rvalue Normalize(const CFloat4Value& rhs);
	CFloatRvalue Saturate(const CFloatValue& rhs);

	CFloatRvalue NewFloat(CShaderBuilder&, float x);

	CFloat2Rvalue NewFloat2(CShaderBuilder& owner, float x, float y);
	CFloat2Rvalue NewFloat2(const CFloatValue& x, const CFloatValue& y);

	CFloat3Rvalue NewFloat3(CShaderBuilder&, float x, float y, float z);
	
	CFloat4Rvalue NewFloat4(CShaderBuilder& owner, float x, float y, float z, float w);
	CFloat4Rvalue NewFloat4(const CFloatValue& x, const CFloat3Value& yzw);
	CFloat4Rvalue NewFloat4(const CFloatValue& x, const CFloatValue& y, const CFloatValue& z, const CFloatValue& w);
	CFloat4Rvalue NewFloat4(const CFloat2Value& xy, const CFloat2Value& zw);
	CFloat4Rvalue NewFloat4(const CFloat3Value& xyz, const CFloatValue& w);

	CIntRvalue NewInt(CShaderBuilder& owner, int32 x);

	CInt2Rvalue NewInt2(CShaderBuilder& owner, int32 x, int32 y);
	CInt2Rvalue NewInt2(const CIntValue& x, const CIntValue& y);

	CUintRvalue NewUint(CShaderBuilder& owner, uint32 x);

	CUint3Rvalue NewUint3(CShaderBuilder& owner, uint32 x, uint32 y, uint32 z);

	CUint4Rvalue NewUint4(CShaderBuilder& owner, uint32 x, uint32 y, uint32 z, uint32 w);
	CUint4Rvalue NewUint4(const CUintValue& x, const CUint3Value& yzw);

	CBoolRvalue NewBool(CShaderBuilder& owner, bool x);

	CFloat4Rvalue Sample(const CTexture2DValue& texture, const CFloat2Value& coord);

	CUint4Rvalue Load(const CImageUint2DValue& image, const CInt2Value& coord);
	void Store(const CImageUint2DValue& image, const CInt2Value& coord, const CUint4Value&);

	CUintRvalue AtomicAnd(const CImageUint2DValue& image, const CInt2Value& coord, const CUintValue&);
	CUintRvalue AtomicOr(const CImageUint2DValue& image, const CInt2Value& coord, const CUintValue&);

	CUintRvalue Load(const CArrayUintValue& buffer, const CIntValue& index);
	void Store(const CArrayUintValue& buffer, const CIntValue& index, const CUintValue&);

	CUintRvalue AtomicAnd(const CArrayUintValue& image, const CIntValue& index, const CUintValue&);
	CUintRvalue AtomicOr(const CArrayUintValue& image, const CIntValue& index, const CUintValue&);

	CFloatRvalue ToFloat(const CUintValue&);
	CFloatRvalue ToFloat(const CIntValue&);
	CFloat2Rvalue ToFloat(const CInt2Value&);
	CFloat4Rvalue ToFloat(const CUint4Value&);
	CIntRvalue ToInt(const CUintValue&);
	CInt2Rvalue ToInt(const CFloat2Value&);
	CUintRvalue ToUint(const CFloatValue&);
	CUintRvalue ToUint(const CIntValue&);
	CUint4Rvalue ToUint(const CFloat4Value&);

	void Return(CShaderBuilder& owner);
	void BeginInvocationInterlock(CShaderBuilder& owner);
	void EndInvocationInterlock(CShaderBuilder& owner);

	void BeginIf(CShaderBuilder& owner, const CBoolValue& condition);
	void EndIf(CShaderBuilder& owner);
}
