#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CIntSwizzleSelector4;
	class CIntRvalue;

	class CIntValue : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CIntValue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CIntLvalue : public CIntValue
	{
	public:
		CIntLvalue(const CShaderBuilder::SYMBOL& symbol)
			: CIntValue(symbol, SWIZZLE_X)
		{

		}

		void operator =(const CIntRvalue& rvalue);
	};

	class CIntRvalue : public CIntValue
	{
	private:
		friend CIntSwizzleSelector4;
		friend CIntRvalue operator /(const CIntValue&, const CIntValue&);
		friend CIntRvalue operator %(const CIntValue&, const CIntValue&);
		friend CIntRvalue NewInt(CShaderBuilder& owner, int32 x);

		CIntRvalue(const CIntRvalue&) = default;

		CIntRvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: CIntValue(symbol, swizzle)
		{

		}

		CIntRvalue& operator =(const CIntRvalue&) = delete;
	};
}
