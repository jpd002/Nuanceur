#pragma once

#include "FloatValue.h"
#include "Float2Value.h"
#include "Float3Value.h"
#include "Float4Value.h"

namespace Nuanceur
{
	class CSwizzleSelector4
	{
	public:
		CSwizzleSelector4(const CShaderBuilder::SYMBOL& symbol)
			: m_symbol(symbol)
		{

		}

		CFloatRvalue x() const
		{
			return CFloatRvalue(m_symbol, SWIZZLE_X);
		}

		CFloatRvalue z() const
		{
			return CFloatRvalue(m_symbol, SWIZZLE_Z);
		}

		CFloat2Rvalue xy() const
		{
			return CFloat2Rvalue(m_symbol, SWIZZLE_XY);
		}

		CFloat2Rvalue zz() const
		{
			return CFloat2Rvalue(m_symbol, SWIZZLE_ZZ);
		}

		CFloat3Rvalue xyz() const
		{
			return CFloat3Rvalue(m_symbol, SWIZZLE_XYZ);
		}

		CFloat4Rvalue xxxx() const
		{
			return CFloat4Rvalue(m_symbol, SWIZZLE_XXXX);
		}

		CFloat4Rvalue xyzw() const
		{
			return CFloat4Rvalue(m_symbol, SWIZZLE_XYZW);
		}

	private:
		CShaderBuilder::SYMBOL m_symbol;
	};
}
