#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CIntSwizzleSelector4;
	class CInt4Rvalue;

	class CInt4Value : public CShaderBuilder::SYMBOLREF
	{
	public:
		CIntSwizzleSelector4* operator ->()
		{
			return m_swizzleSelector.get();
		}

	protected:
		CInt4Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZW)
			: SYMBOLREF(symbol, swizzle)
		{
			m_swizzleSelector = std::make_shared<CIntSwizzleSelector4>(*this);
		}

	private:
		std::shared_ptr<CIntSwizzleSelector4> m_swizzleSelector;
	};

	class CInt4Lvalue : public CInt4Value
	{
	public:
		CInt4Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CInt4Value(symbol, SWIZZLE_XYZW)
		{

		}

		void operator =(const CInt4Rvalue& rvalue);
	};

	class CInt4Rvalue : public CInt4Value
	{
	private:
		friend CIntSwizzleSelector4;

		CInt4Rvalue(const CInt4Rvalue&) = default;

		CInt4Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZW)
			: CInt4Value(symbol, swizzle)
		{

		}

		CInt4Rvalue& operator =(const CInt4Rvalue&) = delete;
	};
}
