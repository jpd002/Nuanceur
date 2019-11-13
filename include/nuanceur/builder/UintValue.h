#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CUintSwizzleSelector4;
	class CArrayUintValue;
	class CUintRvalue;

	class CUintValue : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CUintValue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CUintLvalue : public CUintValue
	{
	public:
		CUintLvalue(const CShaderBuilder::SYMBOL& symbol)
			: CUintValue(symbol, SWIZZLE_X)
		{

		}

		void operator =(const CUintRvalue& rvalue);
	};

	class CUintRvalue : public CUintValue
	{
	private:
		friend CUintSwizzleSelector4;
		friend CUintRvalue Load(const CArrayUintValue&, const CIntValue&);
		friend CUintRvalue operator <<(const CUintValue&, const CUintValue&);
		friend CUintRvalue operator >>(const CUintValue&, const CUintValue&);
		friend CUintRvalue operator &(const CUintValue&, const CUintValue&);
		friend CUintRvalue operator |(const CUintValue&, const CUintValue&);
		friend CUintRvalue NewUint(CShaderBuilder&, uint32);
		friend CUintRvalue ToUint(const CFloatValue&);

		CUintRvalue(const CUintRvalue&) = default;

		CUintRvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
			: CUintValue(symbol, swizzle)
		{

		}

		CUintRvalue& operator =(const CUintRvalue&) = delete;
	};
}
