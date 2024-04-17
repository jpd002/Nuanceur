#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CFloat2Rvalue;
	class CFloatSwizzleSelector4;
	class CInt2Value;
	class CBool2Value;

	class CFloat2Value : public CShaderBuilder::SYMBOLREF
	{
	public:
		CFloatSwizzleSelector4* operator ->()
		{
			assert(swizzle == SWIZZLE_XY);
			return m_swizzleSelector.get();
		}

	protected:
		CFloat2Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XY)
			: SYMBOLREF(symbol, swizzle)
		{
			m_swizzleSelector = std::make_shared<CFloatSwizzleSelector4>(symbol);
		}

	private:
		std::shared_ptr<CFloatSwizzleSelector4> m_swizzleSelector;
	};

	class CFloat2Lvalue : public CFloat2Value
	{
	public:
		CFloat2Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CFloat2Value(symbol, SWIZZLE_XY)
		{

		}

		void operator =(const CFloat2Lvalue& lvalue) = delete;
		void operator =(const CFloat2Rvalue& rvalue);
	};

	class CFloat2Rvalue : public CFloat2Value
	{
	private:
		friend CFloatSwizzleSelector4;
		friend CFloat2Rvalue operator +(const CFloat2Value&, const CFloat2Value&);
		friend CFloat2Rvalue operator *(const CFloat2Value&, const CFloat2Value&);
		friend CFloat2Rvalue operator /(const CFloat2Value&, const CFloat2Value&);
		friend CFloat2Rvalue Fract(const CFloat2Value&);
		friend CFloat2Rvalue Mix(const CFloat2Value&, const CFloat2Value&, const CBool2Value&);
		friend CFloat2Rvalue NewFloat2(CShaderBuilder&, float, float);
		friend CFloat2Rvalue NewFloat2(const CFloatValue&, const CFloatValue&);
		friend CFloat2Rvalue ToFloat(const CInt2Value&);

		CFloat2Rvalue(const CFloat2Rvalue&) = default;

		CFloat2Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XY)
			: CFloat2Value(symbol, swizzle)
		{

		}

		CFloat2Rvalue& operator =(const CFloat2Rvalue&) = delete;
	};
}
