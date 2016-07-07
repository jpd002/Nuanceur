#pragma once

#include "ShaderBuilder.h"
#include <memory>

namespace Nuanceur
{
	class CFloat3Value;
	class CFloat4Rvalue;
	class CTexture2DValue;
	class CMatrix44Value;
	class CSwizzleSelector4;

	class CFloat4Value : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CFloat4Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZW)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CFloat4Lvalue : public CFloat4Value
	{
	public:
		CFloat4Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CFloat4Value(symbol, SWIZZLE_XYZW)
		{
			m_swizzleSelector = std::make_unique<CSwizzleSelector4>(symbol);
		}

		void operator =(const CFloat4Rvalue& rvalue);

		CSwizzleSelector4* operator ->()
		{
			return m_swizzleSelector.get();
		}

	private:
		std::unique_ptr<CSwizzleSelector4> m_swizzleSelector;
	};

	class CFloat4Rvalue : public CFloat4Value
	{
	private:
		friend CSwizzleSelector4;
		friend CFloat4Rvalue operator +(const CFloat4Value&, const CFloat4Value&);
		friend CFloat4Rvalue operator -(const CFloat4Value&, const CFloat4Value&);
		friend CFloat4Rvalue operator *(const CMatrix44Value&, const CFloat4Value&);
		friend CFloat4Rvalue NewFloat4(CShaderBuilder&, float, float, float, float);
		friend CFloat4Rvalue NewFloat4(const CFloat3Value&, float);
		friend CFloat4Rvalue Normalize(const CFloat4Value&);
		friend CFloat4Rvalue Sample(const CTexture2DValue&, const CFloat4Value&);

		CFloat4Rvalue(const CFloat4Rvalue&) = default;

		CFloat4Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZW)
			: CFloat4Value(symbol, swizzle)
		{

		}

		CFloat4Rvalue& operator =(const CFloat4Rvalue&) = delete;
	};
}
