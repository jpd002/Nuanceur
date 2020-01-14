#pragma once

#include "ShaderBuilder.h"
#include <memory>

namespace Nuanceur
{
	class CIntValue;
	class CUintValue;
	class CFloatRvalue;
	class CFloatSwizzleSelector;
	class CFloatSwizzleSelector4;

	class CFloatValue : public CShaderBuilder::SYMBOLREF
	{
	public:
		CFloatSwizzleSelector* operator ->()
		{
			return m_swizzleSelector.get();
		}

	protected:
		CFloatValue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: SYMBOLREF(symbol, swizzle)
		{
			m_swizzleSelector = std::make_shared<CFloatSwizzleSelector>(symbol);
		}

	private:
		std::shared_ptr<CFloatSwizzleSelector> m_swizzleSelector;
	};

	class CFloatLvalue : public CFloatValue
	{
	public:
		CFloatLvalue(const CShaderBuilder::SYMBOL& symbol)
			: CFloatValue(symbol, SWIZZLE_X)
		{

		}

		void operator =(const CFloatRvalue& rvalue);
	};

	class CFloatRvalue : public CFloatValue
	{
	private:
		friend CFloatSwizzleSelector;
		friend CFloatSwizzleSelector4;
		friend CFloatRvalue operator +(const CFloatValue&, const CFloatValue&);
		friend CFloatRvalue operator *(const CFloatValue&, const CFloatValue&);
		friend CFloatRvalue operator /(const CFloatValue&, const CFloatValue&);
		friend CFloatRvalue Mix(const CFloatValue&, const CFloatValue&, const CFloatValue&);
		friend CFloatRvalue NewFloat(CShaderBuilder&, float);
		friend CFloatRvalue Saturate(const CFloatValue&);
		friend CFloatRvalue ToFloat(const CIntValue&);
		friend CFloatRvalue ToFloat(const CUintValue&);

		CFloatRvalue(const CFloatRvalue&) = default;

		CFloatRvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: CFloatValue(symbol, swizzle)
		{

		}

		CFloatRvalue& operator =(const CFloatRvalue&) = delete;
	};
}
