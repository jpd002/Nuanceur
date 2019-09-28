#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CFloat2Value;
	class CInt2Rvalue;

	class CInt2Value : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CInt2Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XY)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CInt2Lvalue : public CInt2Value
	{
	public:
		CInt2Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CInt2Value(symbol, SWIZZLE_XY)
		{

		}

		void operator =(const CInt2Rvalue& rvalue);
	};

	class CInt2Rvalue : public CInt2Value
	{
	private:
		friend CInt2Rvalue operator +(const CInt2Value&, const CInt2Value&);
		friend CInt2Rvalue operator *(const CInt2Value&, const CInt2Value&);
		friend CInt2Rvalue operator /(const CInt2Value&, const CInt2Value&);
		friend CInt2Rvalue NewInt2(CShaderBuilder&, int32, int32);
		friend CInt2Rvalue ToInt(const CFloat2Value&);

		CInt2Rvalue(const CInt2Rvalue&) = default;

		CInt2Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XY)
			: CInt2Value(symbol, swizzle)
		{

		}

		CInt2Rvalue& operator =(const CInt2Rvalue&) = delete;
	};
}
