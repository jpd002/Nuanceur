#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CImageUint2DValue;
	class CInt2Value;
	class CUintSwizzleSelector4;
	class CUint4Rvalue;

	class CUint4Value : public CShaderBuilder::SYMBOLREF
	{
	public:
		CUintSwizzleSelector4* operator ->()
		{
			return m_swizzleSelector.get();
		}

	protected:
		CUint4Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZW)
			: SYMBOLREF(symbol, swizzle)
		{
			m_swizzleSelector = std::make_shared<CUintSwizzleSelector4>(symbol);
		}

	private:
		std::shared_ptr<CUintSwizzleSelector4> m_swizzleSelector;
	};

	class CUint4Lvalue : public CUint4Value
	{
	public:
		CUint4Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CUint4Value(symbol, SWIZZLE_XYZW)
		{

		}

		void operator =(const CUint4Rvalue& rvalue);
	};

	class CUint4Rvalue : public CUint4Value
	{
	private:
		friend CUintSwizzleSelector4;
		friend CUint4Rvalue Load(const CImageUint2DValue&, const CInt2Value&);
		friend CUint4Rvalue NewUint4(CShaderBuilder&, uint32, uint32, uint32, uint32);
		friend CUint4Rvalue NewUint4(const CUintValue&, const CUint3Value&);
		friend CUint4Rvalue ToUint(const CFloat4Value&);

		CUint4Rvalue(const CUint4Rvalue&) = default;

		CUint4Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZW)
			: CUint4Value(symbol, swizzle)
		{

		}

		CUint4Rvalue& operator =(const CUint4Rvalue&) = delete;
	};
}
