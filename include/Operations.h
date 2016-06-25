#pragma once

#include "ShaderBuilder.h"
#include "Float3Value.h"
#include "Float4Value.h"

namespace Nuanceur
{
	CFloat3Rvalue operator +(const CFloat3Value& lhs, const CFloat3Value& rhs);
	CFloat4Rvalue operator +(const CFloat4Value& lhs, const CFloat4Value& rhs);

	CFloat4Rvalue operator -(const CFloat4Value& lhs, const CFloat4Value& rhs);

	CFloat4Rvalue NewFloat4(CShaderBuilder& owner, float cstX, float cstY, float cstZ, float cstW);
	CFloat4Rvalue NewFloat4(const CFloat3Value& rhs, float constant);

	CFloat4Rvalue Normalize(const CFloat4Value& rhs);
}
