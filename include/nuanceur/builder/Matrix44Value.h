#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CMatrix44Value : public CShaderBuilder::SYMBOLREF
	{
	public:
		CMatrix44Value(const CShaderBuilder::SYMBOL& symbol)
		    : SYMBOLREF(symbol, SWIZZLE_XYZW)
		{
		}
	};
}
