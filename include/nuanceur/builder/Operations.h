#pragma once

#include "ShaderBuilder.h"
#include "FloatValue.h"
#include "Float2Value.h"
#include "Float3Value.h"
#include "Float4Value.h"
#include "Matrix44Value.h"
#include "Texture2DValue.h"

namespace Nuanceur
{
	CFloat3Rvalue operator +(const CFloat3Value& lhs, const CFloat3Value& rhs);
	CFloat4Rvalue operator +(const CFloat4Value& lhs, const CFloat4Value& rhs);

	CFloat4Rvalue operator -(const CFloat4Value& lhs, const CFloat4Value& rhs);

	CFloatRvalue operator *(const CFloatValue& lhs, const CFloatValue& rhs);
	CFloat4Rvalue operator *(const CFloat4Value& lhs, const CFloat4Value& rhs);

	CFloatRvalue operator /(const CFloatValue& lhs, const CFloatValue& rhs);

	CFloat4Rvalue operator *(const CMatrix44Value& lhs, const CFloat4Value& rhs);

	CFloatRvalue NewFloat(CShaderBuilder&, float x);

	CFloat2Rvalue NewFloat2(const CFloatValue& x, float y);

	CFloat4Rvalue NewFloat4(CShaderBuilder& owner, float x, float y, float z, float w);
	CFloat4Rvalue NewFloat4(const CFloat3Value& xyz, float w);

	CFloat4Rvalue Normalize(const CFloat4Value& rhs);

	CFloat4Rvalue Sample(const CTexture2DValue& texture, const CFloat2Value& coord);
}
