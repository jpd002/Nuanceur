#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CFloatValue;
	class CIntValue;
	class CUintValue;
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
		friend CBoolRvalue operator &&(const CBoolValue&, const CBoolValue&);
		friend CBoolRvalue operator ||(const CBoolValue&, const CBoolValue&);
		friend CBoolRvalue operator !(const CBoolValue&);
		friend CBoolRvalue operator ==(const CFloatValue&, const CFloatValue&);
		friend CBoolRvalue operator <(const CFloatValue&, const CFloatValue&);
		friend CBoolRvalue operator <(const CUintValue&, const CUintValue&);
		friend CBoolRvalue operator >=(const CIntValue&, const CIntValue&);
		friend CBoolRvalue operator ==(const CUintValue&, const CUintValue&);
		friend CBoolRvalue operator !=(const CUintValue&, const CUintValue&);
		friend CBoolRvalue operator >(const CUintValue&, const CUintValue&);
		friend CBoolRvalue operator >=(const CUintValue&, const CUintValue&);
		friend CBoolRvalue NewBool(CShaderBuilder&, bool);

		CBoolRvalue(const CBoolRvalue&) = default;

		CBoolRvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: CBoolValue(symbol, swizzle)
		{

		}

		CBoolRvalue& operator =(const CBoolRvalue&) = delete;
	};
}
