#pragma once

#include "ShaderBuilder.h"

namespace Nuanceur
{
	class CIntSwizzleSelector4;
	class CInt4Rvalue;
	class CFloat4Value;

	class CInt4Value : public CShaderBuilder::SYMBOLREF
	{
	public:
		CIntSwizzleSelector4* operator ->()
		{
			return m_swizzleSelector.get();
		}

	protected:
		CInt4Value(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZW)
			: SYMBOLREF(symbol, swizzle)
		{
			m_swizzleSelector = std::make_shared<CIntSwizzleSelector4>(*this);
		}

	private:
		std::shared_ptr<CIntSwizzleSelector4> m_swizzleSelector;
	};

	class CInt4Lvalue : public CInt4Value
	{
	public:
		CInt4Lvalue(const CShaderBuilder::SYMBOL& symbol)
			: CInt4Value(symbol, SWIZZLE_XYZW)
		{

		}

		void operator =(const CInt4Lvalue& lvalue) = delete;
		void operator =(const CInt4Rvalue& rvalue);
	};

	class CInt4Rvalue : public CInt4Value
	{
	private:
		friend CIntSwizzleSelector4;
		friend CInt4Rvalue operator +(const CInt4Value&, const CInt4Value&);
		friend CInt4Rvalue operator &(const CInt4Value&, const CInt4Value&);
		friend CInt4Rvalue Clamp(const CInt4Value&, const CInt4Value&, const CInt4Value&);
		friend CInt4Rvalue NewInt4(CShaderBuilder&, int32, int32, int32, int32);
		friend CInt4Rvalue NewInt4(const CIntValue&, const CIntValue&, const CIntValue&, const CIntValue&);
		friend CInt4Rvalue NewInt4(const CInt3Value&, const CIntValue&);
		friend CInt4Rvalue ToInt(const CFloat4Value&);

		CInt4Rvalue(const CInt4Rvalue&) = default;

		CInt4Rvalue(const CShaderBuilder::SYMBOL& symbol, SWIZZLE_TYPE swizzle = SWIZZLE_XYZW)
			: CInt4Value(symbol, swizzle)
		{

		}

		CInt4Rvalue& operator =(const CInt4Rvalue&) = delete;
	};
}
