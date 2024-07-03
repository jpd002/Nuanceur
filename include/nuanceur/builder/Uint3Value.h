#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CUintSwizzleSelector4;
	class CUint3Rvalue;

	class CUint3Value : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CUint3Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZ)
		    : SYMBOLREF(symbol, swizzle)
		{
		}
	};

	class CUint3Lvalue : public CUint3Value
	{
	public:
		CUint3Lvalue(const CShaderBuilder::SYMBOL& symbol)
		    : CUint3Value(symbol, SWIZZLE_XYZ)
		{
		}

		void operator=(const CUint3Lvalue& lvalue) = delete;
		void operator=(const CUint3Rvalue& rvalue);
	};

	class CUint3Rvalue : public CUint3Value
	{
	private:
		friend CUintSwizzleSelector4;
		friend CUint3Rvalue operator+(const CUint3Value&, const CUint3Value&);
		friend CUint3Rvalue operator-(const CUint3Value&, const CUint3Value&);
		friend CUint3Rvalue operator*(const CUint3Value&, const CUint3Value&);
		friend CUint3Rvalue operator>>(const CUint3Value&, const CUint3Value&);
		friend CUint3Rvalue NewUint3(CShaderBuilder&, uint32, uint32, uint32);

		CUint3Rvalue(const CUint3Rvalue&) = default;

		CUint3Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZ)
		    : CUint3Value(symbol, swizzle)
		{
		}

		CUint3Rvalue& operator=(const CUint3Rvalue&) = delete;
	};
}
