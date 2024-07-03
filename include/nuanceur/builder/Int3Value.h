#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CIntSwizzleSelector4;
	class CInt3Rvalue;

	class CInt3Value : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CInt3Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZ)
		    : SYMBOLREF(symbol, swizzle)
		{
		}
	};

	class CInt3Lvalue : public CInt3Value
	{
	public:
		CInt3Lvalue(const CShaderBuilder::SYMBOL& symbol)
		    : CInt3Value(symbol, SWIZZLE_XYZ)
		{
		}

		void operator=(const CInt3Lvalue& lvalue) = delete;
		void operator=(const CInt3Rvalue& rvalue);
	};

	class CInt3Rvalue : public CInt3Value
	{
	private:
		friend CIntSwizzleSelector4;
		friend CInt3Rvalue operator+(const CInt3Value&, const CInt3Value&);
		friend CInt3Rvalue operator-(const CInt3Value&, const CInt3Value&);
		friend CInt3Rvalue operator*(const CInt3Value&, const CInt3Value&);
		friend CInt3Rvalue ShiftRightArithmetic(const CInt3Value&, const CInt3Value&);
		friend CInt3Rvalue NewInt3(CShaderBuilder&, int32, int32, int32);

		CInt3Rvalue(const CInt3Rvalue&) = default;

		CInt3Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZ)
		    : CInt3Value(symbol, swizzle)
		{
		}

		CInt3Rvalue& operator=(const CInt3Rvalue&) = delete;
	};
}
