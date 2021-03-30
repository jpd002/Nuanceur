#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CUshortRvalue;

	class CUshortValue : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CUshortValue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CUshortLvalue : public CUshortValue
	{
	public:
		CUshortLvalue(const CShaderBuilder::SYMBOL& symbol)
			: CUshortValue(symbol, SWIZZLE_X)
		{

		}

	};

	class CUshortRvalue : public CUshortValue
	{
	private:
		friend CUshortRvalue ToUshort(const CUintValue&);

		CUshortRvalue(const CUshortRvalue&) = default;

		CUshortRvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: CUshortValue(symbol, swizzle)
		{

		}

		CUshortRvalue& operator =(const CUshortRvalue&) = delete;
	};
}
