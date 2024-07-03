#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CArrayUintValue : public CShaderBuilder::SYMBOLREF
	{
	public:
		CArrayUintValue(const CShaderBuilder::SYMBOL& symbol)
		    : SYMBOLREF(symbol, SWIZZLE_XYZW)
		{
		}
	};
}
