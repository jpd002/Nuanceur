#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CUcharRvalue;

	class CUcharValue : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CUcharValue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CUcharLvalue : public CUcharValue
	{
	public:
		CUcharLvalue(const CShaderBuilder::SYMBOL& symbol)
			: CUcharValue(symbol, SWIZZLE_X)
		{

		}

	};

	class CUcharRvalue : public CUcharValue
	{
	private:
		friend CUcharRvalue ToUchar(const CUintValue&);

		CUcharRvalue(const CUcharRvalue&) = default;

		CUcharRvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: CUcharValue(symbol, swizzle)
		{

		}

		CUcharRvalue& operator =(const CUcharRvalue&) = delete;
	};
}
