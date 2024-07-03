#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CIntSwizzleSelector4;
	class CIntRvalue;

	class CIntValue : public CShaderBuilder::SYMBOLREF
	{
	public:
		CIntSwizzleSelector4* operator->()
		{
			return m_swizzleSelector.get();
		}

	protected:
		CIntValue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
		    : SYMBOLREF(symbol, swizzle)
		{
			m_swizzleSelector = std::make_shared<CIntSwizzleSelector4>(*this);
		}

	private:
		std::shared_ptr<CIntSwizzleSelector4> m_swizzleSelector;
	};

	class CIntLvalue : public CIntValue
	{
	public:
		CIntLvalue(const CShaderBuilder::SYMBOL& symbol)
		    : CIntValue(symbol, SWIZZLE_X)
		{
		}

		void operator=(const CIntLvalue& lvalue) = delete;
		void operator=(const CIntRvalue& rvalue);
	};

	class CIntRvalue : public CIntValue
	{
	private:
		friend CIntSwizzleSelector4;
		friend CIntRvalue operator+(const CIntValue&, const CIntValue&);
		friend CIntRvalue operator-(const CIntValue&, const CIntValue&);
		friend CIntRvalue operator*(const CIntValue&, const CIntValue&);
		friend CIntRvalue operator/(const CIntValue&, const CIntValue&);
		friend CIntRvalue operator%(const CIntValue&, const CIntValue&);
		friend CIntRvalue operator<<(const CIntValue&, const CIntValue&);
		friend CIntRvalue operator>>(const CIntValue&, const CIntValue&);
		friend CIntRvalue operator&(const CIntValue&, const CIntValue&);
		friend CIntRvalue operator|(const CIntValue&, const CIntValue&);
		friend CIntRvalue Clamp(const CIntValue&, const CIntValue&, const CIntValue&);
		friend CIntRvalue Min(const CIntValue&, const CIntValue&);
		friend CIntRvalue NewInt(CShaderBuilder& owner, int32 x);
		friend CIntRvalue ToInt(const CFloatValue&);
		friend CIntRvalue ToInt(const CUintValue&);

		CIntRvalue(const CIntRvalue&) = default;

		CIntRvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
		    : CIntValue(symbol, swizzle)
		{
		}

		CIntRvalue& operator=(const CIntRvalue&) = delete;
	};
}
