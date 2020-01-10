#pragma once

#include "FloatValue.h"
#include "Float2Value.h"
#include "Float3Value.h"
#include "Float4Value.h"

namespace Nuanceur
{
	class CFloatSwizzleSelector4
	{
	public:
		CFloatSwizzleSelector4(const CShaderBuilder::SYMBOL& symbol)
			: m_symbol(symbol)
		{

		}

		CFloatRvalue x() const
		{
			return CFloatRvalue(m_symbol, SWIZZLE_X);
		}

		CFloatRvalue y() const
		{
			return CFloatRvalue(m_symbol, SWIZZLE_Y);
		}

		CFloatRvalue z() const
		{
			return CFloatRvalue(m_symbol, SWIZZLE_Z);
		}

		CFloatRvalue w() const
		{
			return CFloatRvalue(m_symbol, SWIZZLE_W);
		}

		CFloat2Rvalue xy() const
		{
			return CFloat2Rvalue(m_symbol, SWIZZLE_XY);
		}

		CFloat2Rvalue zz() const
		{
			return CFloat2Rvalue(m_symbol, SWIZZLE_ZZ);
		}
		
		CFloat2Rvalue ww() const
		{
			return CFloat2Rvalue(m_symbol, SWIZZLE_WW);
		}

		CFloat3Rvalue xyz() const
		{
			return CFloat3Rvalue(m_symbol, SWIZZLE_XYZ);
		}

		CFloat3Rvalue www() const
		{
			return CFloat3Rvalue(m_symbol, SWIZZLE_WWW);
		}

		CFloat4Rvalue xxxx() const
		{
			return CFloat4Rvalue(m_symbol, SWIZZLE_XXXX);
		}

		CFloat4Rvalue xyzw() const
		{
			return CFloat4Rvalue(m_symbol, SWIZZLE_XYZW);
		}

		CFloat4Rvalue wwww() const
		{
			return CFloat4Rvalue(m_symbol, SWIZZLE_WWWW);
		}

	private:
		CShaderBuilder::SYMBOL m_symbol;
	};
}
