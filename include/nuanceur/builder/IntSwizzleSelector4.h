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
		CIntSwizzleSelector4(const CShaderBuilder::SYMBOLREF& symbolRef)
			: m_symbolRef(symbolRef)
		{

		}

		CIntRvalue x() const
		{
			return CIntRvalue(m_symbolRef.symbol, TransformSwizzle(m_symbolRef.swizzle, SWIZZLE_X));
		}

		CIntRvalue y() const
		{
			return CIntRvalue(m_symbolRef.symbol, TransformSwizzle(m_symbolRef.swizzle, SWIZZLE_Y));
		}

		CIntRvalue z() const
		{
			return CIntRvalue(m_symbolRef.symbol, TransformSwizzle(m_symbolRef.swizzle, SWIZZLE_Z));
		}

		CIntRvalue w() const
		{
			return CIntRvalue(m_symbolRef.symbol, TransformSwizzle(m_symbolRef.swizzle, SWIZZLE_W));
		}

		CInt2Rvalue xy() const
		{
			return CInt2Rvalue(m_symbolRef.symbol, TransformSwizzle(m_symbolRef.swizzle, SWIZZLE_XY));
		}

		CInt2Rvalue zw() const
		{
			return CInt2Rvalue(m_symbolRef.symbol, TransformSwizzle(m_symbolRef.swizzle, SWIZZLE_ZW));
		}

	private:
		CShaderBuilder::SYMBOLREF m_symbolRef;
	};
}
