#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CTexture2DValue : public CShaderBuilder::SYMBOLREF
	{
	public:
		CTexture2DValue(const CShaderBuilder::SYMBOL& symbol)
			: SYMBOLREF(symbol, SWIZZLE_XYZW)
		{

		}
	};
}
