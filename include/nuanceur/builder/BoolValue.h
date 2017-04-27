#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CFloatValue;
	class CBoolRvalue;

	class CBoolValue : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CBoolValue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CBoolLvalue : public CBoolValue
	{
	public:
		CBoolLvalue(const CShaderBuilder::SYMBOL& symbol)
			: CBoolValue(symbol, SWIZZLE_X)
		{

		}

		void operator =(const CBoolRvalue& rvalue);
	};

	class CBoolRvalue : public CBoolValue
	{
	private:
		friend CBoolRvalue operator <(const CFloatValue&, const CFloatValue&);

		CBoolRvalue(const CBoolRvalue&) = default;

		CBoolRvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: CBoolValue(symbol, swizzle)
		{

		}

		CBoolRvalue& operator =(const CBoolRvalue&) = delete;
	};
}
