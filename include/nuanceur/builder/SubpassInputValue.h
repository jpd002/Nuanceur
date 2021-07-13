#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CSubpassInputValue : public CShaderBuilder::SYMBOLREF
	{
	public:
		CSubpassInputValue(const CShaderBuilder::SYMBOL& symbol)
			: SYMBOLREF(symbol, SWIZZLE_XYZW)
		{

		}
	};

	class CSubpassInputUintValue : public CShaderBuilder::SYMBOLREF
	{
	public:
		CSubpassInputUintValue(const CShaderBuilder::SYMBOL& symbol)
			: SYMBOLREF(symbol, SWIZZLE_XYZW)
		{

		}
	};
}
