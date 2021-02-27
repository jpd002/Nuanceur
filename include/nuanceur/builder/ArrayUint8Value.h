#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CArrayUint8Value : public CShaderBuilder::SYMBOLREF
	{
	public:
		CArrayUint8Value(const CShaderBuilder::SYMBOL& symbol)
			: SYMBOLREF(symbol, SWIZZLE_XYZW)
		{

		}
	};
}
