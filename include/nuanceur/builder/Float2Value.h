#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CFloat2Rvalue;
	class CSwizzleSelector4;

	class CFloat2Value : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CFloat2Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XY)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CFloat2Lvalue : public CFloat2Value
	{
	public:
		CFloat2Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CFloat2Value(symbol, SWIZZLE_XYZ)
		{

		}

		void operator =(const CFloat2Rvalue& rvalue);
	};

	class CFloat2Rvalue : public CFloat2Value
	{
	private:
		friend CSwizzleSelector4;

		CFloat2Rvalue(const CFloat2Rvalue&) = default;

		CFloat2Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XY)
			: CFloat2Value(symbol, swizzle)
		{

		}

		CFloat2Rvalue& operator =(const CFloat2Rvalue&) = delete;
	};
}
