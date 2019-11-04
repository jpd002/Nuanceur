#pragma once

#include "UintValue.h"
//#include "Uint2Value.h"
#include "Uint3Value.h"
#include "Uint4Value.h"

namespace Nuanceur
{
	class CUintSwizzleSelector4
	{
	public:
		CUintSwizzleSelector4(const CShaderBuilder::SYMBOL& symbol)
			: m_symbol(symbol)
		{

		}

		CUintRvalue x() const
		{
			return CUintRvalue(m_symbol, SWIZZLE_X);
		}

		CUintRvalue y() const
		{
			return CUintRvalue(m_symbol, SWIZZLE_Y);
		}

		CUintRvalue z() const
		{
			return CUintRvalue(m_symbol, SWIZZLE_Z);
		}

		CUintRvalue w() const
		{
			return CUintRvalue(m_symbol, SWIZZLE_W);
		}

		CUint3Rvalue xyz() const
		{
			return CUint3Rvalue(m_symbol, SWIZZLE_XYZ);
		}

		CUint4Rvalue xxxx() const
		{
			return CUint4Rvalue(m_symbol, SWIZZLE_XXXX);
		}

		CUint4Rvalue xyzw() const
		{
			return CUint4Rvalue(m_symbol, SWIZZLE_XYZW);
		}

	private:
		CShaderBuilder::SYMBOL m_symbol;
	};
}
