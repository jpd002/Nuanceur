#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CUintSwizzleSelector4;
	class CImageUint2DValue;
	class CArrayUintValue;
	class CUintRvalue;

	class CUintValue : public CShaderBuilder::SYMBOLREF
	{
	public:
		CUintSwizzleSelector4* operator->()
		{
			assert(swizzle == SWIZZLE_X);
			return m_swizzleSelector.get();
		}

	protected:
		CUintValue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
		    : SYMBOLREF(symbol, swizzle)
		{
			m_swizzleSelector = std::make_shared<CUintSwizzleSelector4>(symbol);
		}

	private:
		std::shared_ptr<CUintSwizzleSelector4> m_swizzleSelector;
	};

	class CUintLvalue : public CUintValue
	{
	public:
		CUintLvalue(const CShaderBuilder::SYMBOL& symbol)
		    : CUintValue(symbol, SWIZZLE_X)
		{
		}

		void operator=(const CUintLvalue& lvalue) = delete;
		void operator=(const CUintRvalue& rvalue);
	};

	class CUintRvalue : public CUintValue
	{
	private:
		friend CUintSwizzleSelector4;
		friend CUintRvalue Load(const CArrayUintValue&, const CIntValue&);
		friend CUintRvalue AtomicAnd(const CArrayUintValue&, const CIntValue&, const CUintValue&);
		friend CUintRvalue AtomicAnd(const CImageUint2DValue&, const CInt2Value&, const CUintValue&);
		friend CUintRvalue AtomicOr(const CArrayUintValue&, const CIntValue&, const CUintValue&);
		friend CUintRvalue AtomicOr(const CImageUint2DValue&, const CInt2Value&, const CUintValue&);
		friend CUintRvalue operator+(const CUintValue&, const CUintValue&);
		friend CUintRvalue operator*(const CUintValue&, const CUintValue&);
		friend CUintRvalue operator<<(const CUintValue&, const CUintValue&);
		friend CUintRvalue operator>>(const CUintValue&, const CUintValue&);
		friend CUintRvalue operator&(const CUintValue&, const CUintValue&);
		friend CUintRvalue operator|(const CUintValue&, const CUintValue&);
		friend CUintRvalue operator^(const CUintValue&, const CUintValue&);
		friend CUintRvalue operator~(const CUintValue&);
		friend CUintRvalue NewUint(CShaderBuilder&, uint32);
		friend CUintRvalue ToUint(const CFloatValue&);
		friend CUintRvalue ToUint(const CIntValue&);

		CUintRvalue(const CUintRvalue&) = default;

		CUintRvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_X)
		    : CUintValue(symbol, swizzle)
		{
		}

		CUintRvalue& operator=(const CUintRvalue&) = delete;
	};
}
