#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CArrayFloat4Value : public CShaderBuilder::SYMBOLREF
	{
	public:
		CArrayFloat4Value(const CShaderBuilder::SYMBOL& symbol)
			: SYMBOLREF(symbol, SWIZZLE_XYZW)
		{

		}
	};
}
