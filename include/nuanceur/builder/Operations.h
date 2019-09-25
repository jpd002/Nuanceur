#pragma once

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

	CFloat4Rvalue Normalize(const CFloat4Value& rhs);

	CFloat4Rvalue Sample(const CTexture2DValue& texture, const CFloat2Value& coord);

	CUint4Rvalue Load(const CImageUint2DValue& image, const CInt2Value& coord);

	CFloatRvalue Saturate(const CFloatValue& rhs);

	void BeginIf(CShaderBuilder& owner, const CBoolValue& condition);
	void EndIf(CShaderBuilder& owner);
}
