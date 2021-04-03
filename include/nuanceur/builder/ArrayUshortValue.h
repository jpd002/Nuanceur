#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CArrayUshortValue : public CShaderBuilder::SYMBOLREF
	{
	public:
		CArrayUshortValue(const CShaderBuilder::SYMBOL& symbol)
			: SYMBOLREF(symbol, SWIZZLE_XYZW)
		{

		}
	};
}
