#pragma once

#include "IntValue.h"
#include "Int2Value.h"
//#include "Int3Value.h"
#include "Int4Value.h"

namespace Nuanceur
{
	class CIntSwizzleSelector4
	{
	public:
		CIntSwizzleSelector4(const CShaderBuilder::SYMBOL& symbol)
			: m_symbol(symbol)
		{

		}

		CIntRvalue x() const
		{
			return CIntRvalue(m_symbol, SWIZZLE_X);
		}

		CIntRvalue y() const
		{
			return CIntRvalue(m_symbol, SWIZZLE_Y);
		}

		CIntRvalue z() const
		{
			return CIntRvalue(m_symbol, SWIZZLE_Z);
		}

		CIntRvalue w() const
		{
			return CIntRvalue(m_symbol, SWIZZLE_W);
		}

		CInt2Rvalue xy() const
		{
			return CInt2Rvalue(m_symbol, SWIZZLE_XY);
		}

		CInt2Rvalue zw() const
		{
			return CInt2Rvalue(m_symbol, SWIZZLE_ZW);
		}

	private:
		CShaderBuilder::SYMBOL m_symbol;
	};
}
