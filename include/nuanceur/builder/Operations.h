#pragma once

#include "Types.h"
#include "ShaderBuilder.h"

#include "BoolValue.h"
#include "FloatValue.h"
#include "Float2Value.h"
#include "Float3Value.h"
#include "Float4Value.h"
#include "ImageUint2DValue.h"
#include "Int2Value.h"
#include "Matrix44Value.h"
#include "Texture2DValue.h"
#include "Uint4Value.h"

namespace Nuanceur
{
	CFloat2Rvalue operator +(const CFloat2Value& lhs, const CFloat2Value& rhs);
	CFloat3Rvalue operator +(const CFloat3Value& lhs, const CFloat3Value& rhs);
	CFloat4Rvalue operator +(const CFloat4Value& lhs, const CFloat4Value& rhs);

	CFloat4Rvalue operator -(const CFloat4Value& lhs, const CFloat4Value& rhs);

	CFloatRvalue operator *(const CFloatValue& lhs, const CFloatValue& rhs);
	CFloat2Rvalue operator *(const CFloat2Value& lhs, const CFloat2Value& rhs);
	CFloat4Rvalue operator *(const CFloat4Value& lhs, const CFloat4Value& rhs);

	CFloatRvalue operator /(const CFloatValue& lhs, const CFloatValue& rhs);
	CFloat2Rvalue operator /(const CFloat2Value& lhs, const CFloat2Value& rhs);
	CFloat4Rvalue operator /(const CFloat4Value& lhs, const CFloat4Value& rhs);

	CFloat4Rvalue operator *(const CMatrix44Value& lhs, const CFloat4Value& rhs);

	CBoolRvalue operator <(const CFloatValue& lhs, const CFloatValue& rhs);

	CFloat4Rvalue Clamp(const CFloat4Value&, const CFloat4Value&, const CFloat4Value&);

	CFloatRvalue NewFloat(CShaderBuilder&, float x);

	CFloat2Rvalue NewFloat2(CShaderBuilder& owner, float x, float y);
	CFloat2Rvalue NewFloat2(const CFloatValue& x, const CFloatValue& y);

	CFloat3Rvalue NewFloat3(CShaderBuilder&, float x, float y, float z);
	
	CFloat4Rvalue NewFloat4(CShaderBuilder& owner, float x, float y, float z, float w);
	CFloat4Rvalue NewFloat4(const CFloatValue& x, const CFloat3Value& yzw);
	CFloat4Rvalue NewFloat4(const CFloat3Value& xyz, const CFloatValue& w);

	CInt2Rvalue NewInt2(CShaderBuilder& owner, int32 x, int32 y);

	CUint4Rvalue NewUint4(CShaderBuilder& owner, uint32 x, uint32 y, uint32 z, uint32 w);

	CFloat4Rvalue Normalize(const CFloat4Value& rhs);

	CFloat4Rvalue Sample(const CTexture2DValue& texture, const CFloat2Value& coord);

	CUint4Rvalue Load(const CImageUint2DValue& image, const CInt2Value& coord);
	void Store(const CImageUint2DValue& image, const CInt2Value& coord, const CUint4Value&);

	CInt2Rvalue ToInt(const CFloat2Value&);
	CFloat4Rvalue ToFloat(const CUint4Value&);

	CFloatRvalue Saturate(const CFloatValue& rhs);

	void BeginIf(CShaderBuilder& owner, const CBoolValue& condition);
	void EndIf(CShaderBuilder& owner);
}
