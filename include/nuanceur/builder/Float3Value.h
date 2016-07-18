#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	//class CFloat3Value;
	class CFloat3Rvalue;
	class CSwizzleSelector4;

	class CFloat3Value : public CShaderBuilder::SYMBOLREF
	{
	protected:
		CFloat3Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZ)
			: SYMBOLREF(symbol, swizzle)
		{

		}
	};

	class CFloat3Lvalue : public CFloat3Value
	{
	public:
		CFloat3Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CFloat3Value(symbol, SWIZZLE_XYZ)
		{
		//	m_swizzleSelector = std::make_unique<CSwizzleSelector4>(symbol);
		}

		void operator =(const CFloat3Rvalue& rvalue);

		//CSwizzleSelector3* operator ->()
		//{
		//	return m_swizzleSelector.get();
		//}

	private:
		//std::unique_ptr<CSwizzleSelector3> m_swizzleSelector;
	};

	class CFloat3Rvalue : public CFloat3Value
	{
	private:
		friend CSwizzleSelector4;
		friend CFloat3Rvalue operator +(const CFloat3Value&, const CFloat3Value&);

		CFloat3Rvalue(const CFloat3Rvalue&) = default;

		CFloat3Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZW)
			: CFloat3Value(symbol, swizzle)
		{

		}

		CFloat3Rvalue& operator =(const CFloat3Rvalue&) = delete;
	};
}
