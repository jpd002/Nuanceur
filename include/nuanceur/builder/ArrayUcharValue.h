#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CArrayUcharValue : public CShaderBuilder::SYMBOLREF
	{
	public:
		CArrayUcharValue(const CShaderBuilder::SYMBOL& symbol)
			: SYMBOLREF(symbol, SWIZZLE_XYZW)
		{

		}
	};
}
