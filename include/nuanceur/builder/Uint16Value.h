#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CUint16Rvalue;

	class CUint16Value : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CUint16Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CUint16Lvalue : public CUint16Value
	{
	public:
		CUint16Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CUint16Value(symbol, SWIZZLE_X)
		{

		}

		// void operator =(const CUint16Rvalue& rvalue);
	};

	class CUint16Rvalue : public CUint16Value
	{
	private:
		// friend CUint16Rvalue NewUint(CShaderBuilder&, uint32);
		friend CUint16Rvalue ToUint16(const CUintValue&);

		CUint16Rvalue(const CUint16Rvalue&) = default;

		CUint16Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: CUint16Value(symbol, swizzle)
		{

		}

		CUint16Rvalue& operator =(const CUint16Rvalue&) = delete;
	};
}
