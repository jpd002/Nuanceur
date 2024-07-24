#pragma once

#include <vector>
#include <list>
#include <unordered_map>
#include <cassert>
#include <string>
#include "Types.h"
#include "math/Vector4.h"

namespace Nuanceur
{
	class CShaderBuilder;

	enum SEMANTIC
	{
		SEMANTIC_NONE,
		SEMANTIC_SYSTEM_POSITION,
		SEMANTIC_SYSTEM_POINTSIZE,
		SEMANTIC_SYSTEM_COLOR,
		SEMANTIC_SYSTEM_VERTEXINDEX,
		SEMANTIC_SYSTEM_GIID,
		SEMANTIC_POSITION,
		SEMANTIC_TEXCOORD,
	};

	enum SYMBOL_ATTRIBUTE
	{
		SYMBOL_ATTRIBUTE_COHERENT = 0x01,
	};

	enum
	{
		UNIFORM_UNIT_PUSHCONSTANT = -1,
	};

	enum COMPONENT
	{
		COMPONENT_X,
		COMPONENT_Y,
		COMPONENT_Z,
		COMPONENT_W
	};

	constexpr uint32 MakeSwizzle1(uint32 comp0)
	{
		return 0x100 | (comp0);
	}

	constexpr uint32 MakeSwizzle2(uint32 comp0, uint32 comp1)
	{
		return 0x200 | (comp0) | (comp1 << 2);
	}

	constexpr uint32 MakeSwizzle3(uint32 comp0, uint32 comp1, uint32 comp2)
	{
		return 0x300 | (comp0) | (comp1 << 2) | (comp2 << 4);
	}

	constexpr uint32 MakeSwizzle4(uint32 comp0, uint32 comp1, uint32 comp2, uint32 comp3)
	{
		return 0x400 | (comp0) | (comp1 << 2) | (comp2 << 4) | (comp3 << 6);
	}

#define DEFINE_SWIZZLE1(a) SWIZZLE_##a = MakeSwizzle1(COMPONENT_##a)
#define DEFINE_SWIZZLE2(a, b) SWIZZLE_##a##b = MakeSwizzle2(COMPONENT_##a, COMPONENT_##b)
#define DEFINE_SWIZZLE3(a, b, c) SWIZZLE_##a##b##c = MakeSwizzle3(COMPONENT_##a, COMPONENT_##b, COMPONENT_##c)
#define DEFINE_SWIZZLE4(a, b, c, d) SWIZZLE_##a##b##c##d = MakeSwizzle4(COMPONENT_##a, COMPONENT_##b, COMPONENT_##c, COMPONENT_##d)

	enum SWIZZLE_TYPE
	{
		DEFINE_SWIZZLE1(X),
		DEFINE_SWIZZLE1(Y),
		DEFINE_SWIZZLE1(Z),
		DEFINE_SWIZZLE1(W),

		DEFINE_SWIZZLE2(X, X),
		DEFINE_SWIZZLE2(X, Y),
		DEFINE_SWIZZLE2(X, Z),
		DEFINE_SWIZZLE2(X, W),

		DEFINE_SWIZZLE2(Y, X),
		DEFINE_SWIZZLE2(Y, Y),
		DEFINE_SWIZZLE2(Y, Z),
		DEFINE_SWIZZLE2(Y, W),

		DEFINE_SWIZZLE2(Z, Z),
		DEFINE_SWIZZLE2(Z, W),

		DEFINE_SWIZZLE2(W, Z),
		DEFINE_SWIZZLE2(W, W),

		DEFINE_SWIZZLE3(X, X, X),
		DEFINE_SWIZZLE3(X, Y, Z),
		DEFINE_SWIZZLE3(X, Y, W),
		DEFINE_SWIZZLE3(X, Z, W),

		DEFINE_SWIZZLE3(Y, Y, Y),
		DEFINE_SWIZZLE3(Y, Z, W),

		DEFINE_SWIZZLE3(W, W, W),

		DEFINE_SWIZZLE4(X, X, X, X),
		DEFINE_SWIZZLE4(X, Y, Z, W),

		DEFINE_SWIZZLE4(Y, X, Z, W),
		DEFINE_SWIZZLE4(Y, Z, W, X),

		DEFINE_SWIZZLE4(W, X, Y, Z),
		DEFINE_SWIZZLE4(W, Z, Y, X),
		DEFINE_SWIZZLE4(W, Z, Z, W),
		DEFINE_SWIZZLE4(W, W, W, W),
	};

#undef DEFINE_SWIZZLE4
#undef DEFINE_SWIZZLE3
#undef DEFINE_SWIZZLE2
#undef DEFINE_SWIZZLE1

	bool IsIdentitySwizzle(SWIZZLE_TYPE);
	bool IsMaskSwizzle(SWIZZLE_TYPE);

	inline uint32 GetSwizzleElement(SWIZZLE_TYPE swizzle, uint32 index)
	{
		uint32 elemCount = (swizzle >> 8);
		assert(index < elemCount);
		return (swizzle >> (index * 2)) & 0x03;
	}

	inline uint32 GetSwizzleElementCount(SWIZZLE_TYPE swizzle)
	{
		uint32 elemCount = (swizzle >> 8);
		assert((elemCount > 0) && (elemCount <= 4));
		return elemCount;
	}

	SWIZZLE_TYPE TransformSwizzle(SWIZZLE_TYPE, SWIZZLE_TYPE);

	class CShaderBuilder
	{
	public:
		enum METADATA_TYPE
		{
			METADATA_LOCALSIZE_X,
			METADATA_LOCALSIZE_Y,
			METADATA_LOCALSIZE_Z,
		};

		enum SYMBOL_TYPE
		{
			SYMBOL_TYPE_NULL,
			SYMBOL_TYPE_FLOAT4,
			SYMBOL_TYPE_INT4,
			SYMBOL_TYPE_UINT4,
			SYMBOL_TYPE_BOOL4,
			SYMBOL_TYPE_MATRIX,
			SYMBOL_TYPE_ARRAYUINT,
			SYMBOL_TYPE_ARRAYUCHAR,
			SYMBOL_TYPE_ARRAYUSHORT,
			SYMBOL_TYPE_TEXTURE2D,
			SYMBOL_TYPE_IMAGE2DUINT,
			SYMBOL_TYPE_SUBPASSINPUT,
			SYMBOL_TYPE_SUBPASSINPUTUINT,
		};

		enum SYMBOL_LOCATION
		{
			SYMBOL_LOCATION_NULL,
			SYMBOL_LOCATION_TEMPORARY,
			SYMBOL_LOCATION_VARIABLE,
			SYMBOL_LOCATION_INPUT,
			SYMBOL_LOCATION_OUTPUT,
			SYMBOL_LOCATION_UNIFORM,
			SYMBOL_LOCATION_TEXTURE,
		};

		struct SEMANTIC_INFO
		{
			SEMANTIC_INFO(SEMANTIC type, unsigned int index)
			    : type(type)
			    , index(index)
			{
			}

			SEMANTIC type = SEMANTIC_NONE;
			unsigned int index = 0;
		};

		struct SYMBOL
		{
			SYMBOL()
			{
			}

			CShaderBuilder* owner = nullptr;
			SYMBOL_TYPE type = SYMBOL_TYPE_NULL;
			SYMBOL_LOCATION location = SYMBOL_LOCATION_NULL;
			unsigned int unit = 0;
			unsigned int index = 0;
			uint32 attributes = 0;
		};

		struct SYMBOLREF
		{
			SYMBOLREF()
			{
			}

			SYMBOLREF(const SYMBOL& symbol, const SWIZZLE_TYPE& swizzle)
			    : symbol(symbol)
			    , swizzle(swizzle)
			{
			}

			SYMBOL symbol;
			SWIZZLE_TYPE swizzle = SWIZZLE_XYZW;
		};

		class CIntVector4
		{
		public:
			CIntVector4(int32 x, int32 y, int32 z, int32 w)
			    : x(x)
			    , y(y)
			    , z(z)
			    , w(w)
			{
			}

			int32 x;
			int32 y;
			int32 z;
			int32 w;
		};

		class CBoolVector4
		{
		public:
			CBoolVector4(bool x, bool y, bool z, bool w)
			    : x(x)
			    , y(y)
			    , z(z)
			    , w(w)
			{
			}

			bool x;
			bool y;
			bool z;
			bool w;
		};

		enum STATEMENT_OP
		{
			STATEMENT_OP_NOP,
			STATEMENT_OP_ADD,
			STATEMENT_OP_SUBSTRACT,
			STATEMENT_OP_MULTIPLY,
			STATEMENT_OP_DIVIDE,
			STATEMENT_OP_MODULO,
			STATEMENT_OP_AND,
			STATEMENT_OP_OR,
			STATEMENT_OP_XOR,
			STATEMENT_OP_NOT,
			STATEMENT_OP_LSHIFT,
			STATEMENT_OP_RSHIFT,
			STATEMENT_OP_RSHIFT_ARITHMETIC,
			STATEMENT_OP_LOGICAL_AND,
			STATEMENT_OP_LOGICAL_OR,
			STATEMENT_OP_LOGICAL_NOT,
			STATEMENT_OP_COMPARE_EQ,
			STATEMENT_OP_COMPARE_NE,
			STATEMENT_OP_COMPARE_LT,
			STATEMENT_OP_COMPARE_LE,
			STATEMENT_OP_COMPARE_GT,
			STATEMENT_OP_COMPARE_GE,
			STATEMENT_OP_DOT,
			STATEMENT_OP_POW,
			STATEMENT_OP_NEGATE,
			STATEMENT_OP_ABS,
			STATEMENT_OP_CLAMP,
			STATEMENT_OP_FRACT,
			STATEMENT_OP_LOG2,
			STATEMENT_OP_MIN,
			STATEMENT_OP_MAX,
			STATEMENT_OP_MIX,
			STATEMENT_OP_NORMALIZE,
			STATEMENT_OP_SATURATE,
			STATEMENT_OP_TRUNC,
			STATEMENT_OP_ISINF,
			STATEMENT_OP_LENGTH,
			STATEMENT_OP_NEWVECTOR2,
			STATEMENT_OP_NEWVECTOR4,
			STATEMENT_OP_ASSIGN,
			STATEMENT_OP_SAMPLE,
			STATEMENT_OP_LOAD,
			STATEMENT_OP_STORE,
			STATEMENT_OP_STORE16,
			STATEMENT_OP_STORE8,
			STATEMENT_OP_ATOMICAND,
			STATEMENT_OP_ATOMICOR,
			STATEMENT_OP_TOFLOAT,
			STATEMENT_OP_TOINT,
			STATEMENT_OP_TOUINT,
			STATEMENT_OP_TOUSHORT,
			STATEMENT_OP_TOUCHAR,
			STATEMENT_OP_RETURN,
			STATEMENT_OP_INVOCATION_INTERLOCK_BEGIN,
			STATEMENT_OP_INVOCATION_INTERLOCK_END,
			STATEMENT_OP_IF_BEGIN,
			STATEMENT_OP_IF_END,
		};

		struct STATEMENT
		{
			STATEMENT() = default;

			STATEMENT(STATEMENT_OP op, SYMBOLREF dstRef, SYMBOLREF src1Ref, SYMBOLREF src2Ref = SYMBOLREF(), SYMBOLREF src3Ref = SYMBOLREF(), SYMBOLREF src4Ref = SYMBOLREF())
			    : op(op)
			    , dstRef(dstRef)
			    , src1Ref(src1Ref)
			    , src2Ref(src2Ref)
			    , src3Ref(src3Ref)
			    , src4Ref(src4Ref)
			{
			}

			STATEMENT_OP op = STATEMENT_OP_NOP;
			SYMBOLREF dstRef;
			SYMBOLREF src1Ref;
			SYMBOLREF src2Ref;
			SYMBOLREF src3Ref;
			SYMBOLREF src4Ref;

			unsigned int GetSourceCount() const
			{
				if(src4Ref.symbol.location != SYMBOL_LOCATION_NULL)
				{
					assert(src1Ref.symbol.location != SYMBOL_LOCATION_NULL);
					assert(src2Ref.symbol.location != SYMBOL_LOCATION_NULL);
					assert(src3Ref.symbol.location != SYMBOL_LOCATION_NULL);
					return 4;
				}
				else if(src3Ref.symbol.location != SYMBOL_LOCATION_NULL)
				{
					assert(src1Ref.symbol.location != SYMBOL_LOCATION_NULL);
					assert(src2Ref.symbol.location != SYMBOL_LOCATION_NULL);
					return 3;
				}
				else if(src2Ref.symbol.location != SYMBOL_LOCATION_NULL)
				{
					assert(src1Ref.symbol.location != SYMBOL_LOCATION_NULL);
					return 2;
				}
				else if(src1Ref.symbol.location != SYMBOL_LOCATION_NULL)
				{
					return 1;
				}
				else
				{
					return 0;
				}
			}
		};

		typedef std::unordered_map<METADATA_TYPE, uint32> MetadataMap;
		typedef std::vector<SYMBOL> SymbolArray;
		typedef std::list<STATEMENT> StatementList;

		//We need a special operator = that will remap symbol owners

		virtual ~CShaderBuilder() = default;

		uint32 GetMetadata(METADATA_TYPE, uint32) const;
		void SetMetadata(METADATA_TYPE, uint32);

		const SymbolArray& GetSymbols() const;
		SEMANTIC_INFO GetInputSemantic(const SYMBOL&) const;
		SEMANTIC_INFO GetOutputSemantic(const SYMBOL&) const;
		std::string GetVariableName(const SYMBOL&) const;
		std::string GetUniformName(const SYMBOL&) const;
		CVector4 GetTemporaryValue(const SYMBOL&) const;
		CIntVector4 GetTemporaryValueInt(const SYMBOL&) const;
		CBoolVector4 GetTemporaryValueBool(const SYMBOL&) const;

		const StatementList& GetStatements() const;
		void InsertStatement(const STATEMENT&);

		SYMBOL CreateInput(SEMANTIC, unsigned int = 0);
		SYMBOL CreateInputInt(SEMANTIC, unsigned int = 0);
		SYMBOL CreateInputUint(SEMANTIC, unsigned int = 0);
		SYMBOL CreateOutput(SEMANTIC, unsigned int = 0);
		SYMBOL CreateOutputUint(SEMANTIC, unsigned int = 0);
		SYMBOL CreateConstant(float, float, float, float);
		SYMBOL CreateConstantInt(int32, int32, int32, int32);
		SYMBOL CreateConstantUint(uint32, uint32, uint32, uint32);
		SYMBOL CreateConstantBool(bool, bool, bool, bool);

		SYMBOL CreateVariableFloat(const std::string&);
		SYMBOL CreateVariableInt(const std::string&);
		SYMBOL CreateVariableUint(const std::string&);
		SYMBOL CreateVariableBool(const std::string&);

		SYMBOL CreateTemporary();
		SYMBOL CreateTemporaryBool();
		SYMBOL CreateTemporaryInt();
		SYMBOL CreateTemporaryUint();

		SYMBOL CreateUniformFloat4(const std::string&, unsigned int = 0);
		SYMBOL CreateUniformInt4(const std::string&, unsigned int = 0);
		SYMBOL CreateUniformMatrix(const std::string&, unsigned int = 0);
		SYMBOL CreateUniformArrayUint(const std::string&, unsigned int = 0, uint32 = 0);
		SYMBOL CreateUniformArrayUchar(const std::string&, unsigned int = 0, uint32 = 0);
		SYMBOL CreateUniformArrayUshort(const std::string&, unsigned int = 0, uint32 = 0);

		SYMBOL CreateTexture2D(unsigned int);

		SYMBOL CreateImage2DUint(unsigned int);

		SYMBOL CreateSubpassInput(unsigned int, unsigned int);
		SYMBOL CreateSubpassInputUint(unsigned int, unsigned int);

		SYMBOL CreateOptionalInput(bool, SEMANTIC, unsigned int = 0);
		SYMBOL CreateOptionalOutput(bool, SEMANTIC, unsigned int = 0);
		SYMBOL CreateOptionalUniformMatrix(bool, const std::string&);

	private:
		typedef std::unordered_map<unsigned int, SEMANTIC_INFO> SemanticMap;
		typedef std::unordered_map<unsigned int, std::string> VariableNameMap;
		typedef std::unordered_map<unsigned int, std::string> UniformNameMap;
		typedef std::unordered_map<unsigned int, CVector4> TemporaryValueMap;
		typedef std::unordered_map<unsigned int, CIntVector4> TemporaryValueIntMap;
		typedef std::unordered_map<unsigned int, CBoolVector4> TemporaryValueBoolMap;

		MetadataMap m_metadata;
		SymbolArray m_symbols;
		StatementList m_statements;
		unsigned int m_currentTempIndex = 0;
		unsigned int m_currentVariableIndex = 0;
		unsigned int m_currentInputIndex = 0;
		unsigned int m_currentOutputIndex = 0;

		SemanticMap m_inputSemantics;
		SemanticMap m_outputSemantics;
		VariableNameMap m_variableNames;
		UniformNameMap m_uniformNames;
		TemporaryValueMap m_temporaryValues;
		TemporaryValueIntMap m_temporaryValuesInt;
		TemporaryValueBoolMap m_temporaryValuesBool;
	};
}
