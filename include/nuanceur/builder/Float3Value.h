#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CFloat3Rvalue;
	class CFloatSwizzleSelector4;

	class CFloat3Value : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CFloat3Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZ)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CFloat3Lvalue : public CFloat3Value
	{
	public:
		CFloat3Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CFloat3Value(symbol, SWIZZLE_XYZ)
		{

		}

		void operator =(const CFloat3Rvalue& rvalue);
	};

	class CFloat3Rvalue : public CFloat3Value
	{
	private:
		friend CFloatSwizzleSelector4;
		friend CFloat3Rvalue operator +(const CFloat3Value&, const CFloat3Value&);
		friend CFloat3Rvalue operator -(const CFloat3Value&, const CFloat3Value&);
		friend CFloat3Rvalue operator *(const CFloat3Value&, const CFloat3Value&);
		friend CFloat3Rvalue NewFloat3(CShaderBuilder&, float, float, float);

		CFloat3Rvalue(const CFloat3Rvalue&) = default;

		CFloat3Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZ)
			: CFloat3Value(symbol, swizzle)
		{

		}

		CFloat3Rvalue& operator =(const CFloat3Rvalue&) = delete;
	};
}
