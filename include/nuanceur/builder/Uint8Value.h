#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CUint8Rvalue;

	class CUint8Value : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CUint8Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CUint8Lvalue : public CUint8Value
	{
	public:
		CUint8Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CUint8Value(symbol, SWIZZLE_X)
		{

		}

		// void operator =(const CUint8Rvalue& rvalue);
	};

	class CUint8Rvalue : public CUint8Value
	{
	private:
		// friend CUint8Rvalue NewUint(CShaderBuilder&, uint32);
		friend CUint8Rvalue ToUint8(const CUintValue&);

		CUint8Rvalue(const CUint8Rvalue&) = default;

		CUint8Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: CUint8Value(symbol, swizzle)
		{

		}

		CUint8Rvalue& operator =(const CUint8Rvalue&) = delete;
	};
}
