#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CFloatRvalue;
	class CSwizzleSelector4;

	class CFloatValue : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CFloatValue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: SYMBOLREF(symbol, swizzle)
		{

		}
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
		friend CSwizzleSelector4;
		friend CFloatRvalue operator *(const CFloatValue&, const CFloatValue&);
		friend CFloatRvalue operator /(const CFloatValue&, const CFloatValue&);
		friend CFloatRvalue NewFloat(CShaderBuilder&, float);

		CFloatRvalue(const CFloatRvalue&) = default;

		CFloatRvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: CFloatValue(symbol, swizzle)
		{

		}

		CFloatRvalue& operator =(const CFloatRvalue&) = delete;
	};
}
