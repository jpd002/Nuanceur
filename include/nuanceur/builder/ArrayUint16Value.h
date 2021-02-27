#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CArrayUint16Value : public CShaderBuilder::SYMBOLREF
	{
	public:
		CArrayUint16Value(const CShaderBuilder::SYMBOL& symbol)
			: SYMBOLREF(symbol, SWIZZLE_XYZW)
		{

		}
	};
}
