#pragma once

#include "FloatValue.h"
#include "Float2Value.h"
#include "Float3Value.h"
#include "Float4Value.h"

namespace Nuanceur
{
	class CFloatSwizzleSelector
	{
	public:
		CFloatSwizzleSelector(const CShaderBuilder::SYMBOL& symbol)
			: m_symbol(symbol)
		{

		}

		CFloatRvalue x() const
		{
			return CFloatRvalue(m_symbol, SWIZZLE_X);
		}

		CFloat3Rvalue xxx() const
		{
			return CFloat3Rvalue(m_symbol, SWIZZLE_XXX);
		}

	private:
		CShaderBuilder::SYMBOL m_symbol;
	};
}
