#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CBool2Rvalue;
	class CFloat2Value;

	class CBool2Value : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CBool2Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XY)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CBool2Lvalue : public CBool2Value
	{
	public:
		CBool2Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CBool2Value(symbol, SWIZZLE_XY)
		{

		}

		void operator =(const CBool2Rvalue& rvalue);
	};

	class CBool2Rvalue : public CBool2Value
	{
	private:
		friend CBool2Rvalue IsInf(const CFloat2Value&);

		CBool2Rvalue(const CBool2Rvalue&) = default;

		CBool2Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XY)
			: CBool2Value(symbol, swizzle)
		{

		}

		CBool2Rvalue& operator =(const CBool2Rvalue&) = delete;
	};
}
