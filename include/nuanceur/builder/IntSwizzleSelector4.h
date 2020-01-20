#pragma once

#include "IntValue.h"
#include "Int2Value.h"
//#include "Int3Value.h"
#include "Int4Value.h"

namespace Nuanceur
{
	static SWIZZLE_TYPE TransformSwizzle(SWIZZLE_TYPE a, SWIZZLE_TYPE b)
	{
		switch(a)
		{
		default:
			assert(false);
		case SWIZZLE_X:
		case SWIZZLE_XY:
		case SWIZZLE_XYZ:
		case SWIZZLE_XYZW:
			//Return b if identity
			return b;
		case SWIZZLE_ZW:
			switch(b)
			{
			default:
				assert(false);
			case SWIZZLE_X:
				return SWIZZLE_Z;
			case SWIZZLE_Y:
				return SWIZZLE_W;
			}
			break;
		}
	}

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
