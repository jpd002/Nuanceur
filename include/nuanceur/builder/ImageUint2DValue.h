#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CImageUint2DValue : public CShaderBuilder::SYMBOLREF
	{
	public:
		CImageUint2DValue(const CShaderBuilder::SYMBOL& symbol)
			: SYMBOLREF(symbol, SWIZZLE_XYZW)
		{

		}
	};
}
