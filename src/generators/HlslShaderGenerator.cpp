#include "nuanceur/generators/HlslShaderGenerator.h"
#include "string_format.h"

using namespace Nuanceur;

CHlslShaderGenerator::CHlslShaderGenerator(const CShaderBuilder& shaderBuilder, uint32 flags)
    : m_shaderBuilder(shaderBuilder)
    , m_flags(flags)
{
}

std::string CHlslShaderGenerator::Generate(const std::string& methodName, const CShaderBuilder& shaderBuilder, uint32 flags)
{
	CHlslShaderGenerator generator(shaderBuilder, flags);
	return generator.Generate(methodName);
}

std::string CHlslShaderGenerator::Generate(const std::string& methodName) const
{
	std::string result;

	result += GenerateInputStruct();
	result += GenerateOutputStruct();
	result += GenerateConstants();
	result += GenerateSamplers();

	result += string_format("OUTPUT %s(INPUT input)\r\n", methodName.c_str());
	result += "{\r\n";
	result += "\tOUTPUT output;\r\n";

	//Write all temps
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEMPORARY) continue;
		auto temporaryValue = m_shaderBuilder.GetTemporaryValue(symbol);
		result += string_format("\tfloat4 %s = float4(%f, %f, %f, %f);\r\n",
		                        MakeSymbolName(symbol).c_str(),
		                        temporaryValue.x, temporaryValue.y, temporaryValue.z, temporaryValue.w);
	}

	for(const auto& statement : m_shaderBuilder.GetStatements())
	{
		const auto& dstRef = statement.dstRef;
		const auto& src1Ref = statement.src1Ref;
		const auto& src2Ref = statement.src2Ref;
		const auto& src3Ref = statement.src3Ref;
		const auto& src4Ref = statement.src4Ref;
		switch(statement.op)
		{
		case CShaderBuilder::STATEMENT_OP_ADD:
			result += string_format("\t%s = %s + %s;\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_SUBSTRACT:
			result += string_format("\t%s = %s - %s;\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_MULTIPLY:
			if(
			    (src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX) ||
			    (src2Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX))
			{
				result += string_format("\t%s = mul(%s, %s);\r\n",
				                        PrintSymbolRef(dstRef).c_str(),
				                        PrintSymbolRef(src1Ref).c_str(),
				                        PrintSymbolRef(src2Ref).c_str());
			}
			else
			{
				result += string_format("\t%s = %s * %s;\r\n",
				                        PrintSymbolRef(dstRef).c_str(),
				                        PrintSymbolRef(src1Ref).c_str(),
				                        PrintSymbolRef(src2Ref).c_str());
			}
			break;
		case CShaderBuilder::STATEMENT_OP_DIVIDE:
			result += string_format("\t%s = %s / %s;\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_MAX:
			result += string_format("\t%s = max(%s, %s);\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_DOT:
			result += string_format("\t%s = dot(%s, %s);\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_POW:
			result += string_format("\t%s = pow(%s, %s);\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_MIX:
			result += string_format("\t%s = lerp(%s, %s, %s);\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str(),
			                        PrintSymbolRef(src3Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_NEWVECTOR2:
			result += string_format("\t%s = float2(%s, %s);\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_NEWVECTOR4:
			switch(statement.GetSourceCount())
			{
			case 2:
				result += string_format("\t%s = float4(%s, %s);\r\n",
				                        PrintSymbolRef(dstRef).c_str(),
				                        PrintSymbolRef(src1Ref).c_str(),
				                        PrintSymbolRef(src2Ref).c_str());
				break;
			case 3:
				result += string_format("\t%s = float4(%s, %s, %s);\r\n",
				                        PrintSymbolRef(dstRef).c_str(),
				                        PrintSymbolRef(src1Ref).c_str(),
				                        PrintSymbolRef(src2Ref).c_str(),
				                        PrintSymbolRef(src3Ref).c_str());
				break;
			case 4:
				result += string_format("\t%s = float4(%s, %s, %s, %s);\r\n",
				                        PrintSymbolRef(dstRef).c_str(),
				                        PrintSymbolRef(src1Ref).c_str(),
				                        PrintSymbolRef(src2Ref).c_str(),
				                        PrintSymbolRef(src3Ref).c_str(),
				                        PrintSymbolRef(src4Ref).c_str());
				break;
			}
			break;
		case CShaderBuilder::STATEMENT_OP_ASSIGN:
			result += string_format("\t%s = %s;\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_NEGATE:
			result += string_format("\t%s = -%s;\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_SATURATE:
			result += string_format("\t%s = saturate(%s);\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_CLAMP:
			result += string_format("\t%s = clamp(%s, %s, %s);\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str(),
			                        PrintSymbolRef(src3Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_NORMALIZE:
			result += string_format("\t%s = normalize(%s);\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_LENGTH:
			result += string_format("\t%s = length(%s);\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_SAMPLE:
			if(m_flags & FLAG_COMBINED_SAMPLER_TEXTURE)
			{
				assert(src1Ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_TEXTURE2D);
				result += string_format("\t%s = tex2D(c_sampler%d, %s);\r\n",
				                        PrintSymbolRef(dstRef).c_str(),
				                        src1Ref.symbol.unit,
				                        PrintSymbolRef(src2Ref).c_str());
			}
			else
			{
				result += string_format("\t%s = c_texture%d.Sample(c_sampler%d, %s);\r\n",
				                        PrintSymbolRef(dstRef).c_str(),
				                        src1Ref.symbol.unit, src1Ref.symbol.unit,
				                        PrintSymbolRef(src2Ref).c_str());
			}
			break;
		default:
			assert(0);
			break;
		}
	}

	result += "\treturn output;\r\n";
	result += "}\r\n";
	return result;
}

std::string CHlslShaderGenerator::GenerateInputStruct() const
{
	std::string result;
	result += "struct INPUT\r\n";
	result += "{\r\n";
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_INPUT) continue;
		auto semantic = m_shaderBuilder.GetInputSemantic(symbol);
		result += string_format("\t%s %s : %s;\r\n",
		                        MakeTypeName(symbol.type).c_str(),
		                        MakeLocalSymbolName(symbol).c_str(),
		                        MakeSemanticName(semantic).c_str());
	}
	result += "};\r\n";
	return result;
}

std::string CHlslShaderGenerator::GenerateOutputStruct() const
{
	std::string result;
	result += "struct OUTPUT\r\n";
	result += "{\r\n";
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_OUTPUT) continue;
		auto semantic = m_shaderBuilder.GetOutputSemantic(symbol);
		result += string_format("\t%s %s : %s;\r\n",
		                        MakeTypeName(symbol.type).c_str(),
		                        MakeLocalSymbolName(symbol).c_str(),
		                        MakeSemanticName(semantic).c_str());
	}
	result += "};\r\n";
	return result;
}

std::string CHlslShaderGenerator::GenerateConstants() const
{
	std::string result;
	result += "cbuffer Constants\r\n";
	result += "{\r\n";
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
		auto constantType = (symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX) ? "matrix" : "float4";
		result += string_format("\t%s %s;\r\n",
		                        constantType, MakeLocalSymbolName(symbol).c_str());
	}
	result += "};\r\n";
	return result;
}

std::string CHlslShaderGenerator::GenerateSamplers() const
{
	std::string result;
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEXTURE) continue;
		result += string_format("%s c_texture%d : register(t%d);\r\n",
		                        MakeTypeName(symbol.type).c_str(),
		                        symbol.unit, symbol.unit);
		result += string_format("SamplerState c_sampler%d : register(s%d);\r\n",
		                        symbol.unit, symbol.unit);
	}
	return result;
}

std::string CHlslShaderGenerator::MakeSymbolName(const CShaderBuilder::SYMBOL& sym) const
{
	switch(sym.location)
	{
	case CShaderBuilder::SYMBOL_LOCATION_TEMPORARY:
		return string_format("t%d", sym.index);
		break;
	case CShaderBuilder::SYMBOL_LOCATION_INPUT:
		return string_format("input.%s", MakeLocalSymbolName(sym).c_str());
		break;
	case CShaderBuilder::SYMBOL_LOCATION_OUTPUT:
		return string_format("output.%s", MakeLocalSymbolName(sym).c_str());
		break;
	case CShaderBuilder::SYMBOL_LOCATION_UNIFORM:
		return MakeLocalSymbolName(sym);
		break;
	default:
		assert(false);
		return "unknown";
		break;
	}
}

std::string CHlslShaderGenerator::MakeLocalSymbolName(const CShaderBuilder::SYMBOL& sym) const
{
	switch(sym.location)
	{
	case CShaderBuilder::SYMBOL_LOCATION_INPUT:
		return string_format("i%d", sym.index);
	case CShaderBuilder::SYMBOL_LOCATION_OUTPUT:
		return string_format("o%d", sym.index);
	case CShaderBuilder::SYMBOL_LOCATION_UNIFORM:
		return m_shaderBuilder.GetUniformName(sym);
	default:
		assert(false);
		return "unknown";
	}
}

std::string CHlslShaderGenerator::MakeSemanticName(CShaderBuilder::SEMANTIC_INFO semantic)
{
	switch(semantic.type)
	{
	case SEMANTIC_POSITION:
		return "POSITION";
	case SEMANTIC_TEXCOORD:
		return string_format("TEXCOORD%d", semantic.index);
	case SEMANTIC_SYSTEM_POSITION:
		return "SV_POSITION";
	case SEMANTIC_SYSTEM_COLOR:
		return "SV_TARGET";
	default:
		assert(false);
		return "";
	}
}

std::string CHlslShaderGenerator::MakeTypeName(CShaderBuilder::SYMBOL_TYPE type)
{
	switch(type)
	{
	case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		return "float4";
	case CShaderBuilder::SYMBOL_TYPE_MATRIX:
		return "matrix";
	case CShaderBuilder::SYMBOL_TYPE_TEXTURE2D:
		return "Texture2D";
	default:
		assert(false);
		return "";
	}
}

std::string CHlslShaderGenerator::PrintSymbolRef(const CShaderBuilder::SYMBOLREF& ref) const
{
	auto symbolName = MakeSymbolName(ref.symbol);
	if(ref.symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX)
	{
		return symbolName;
	}
	switch(ref.swizzle)
	{
	case SWIZZLE_X:
		return symbolName + ".x";
		break;
	case SWIZZLE_Y:
		return symbolName + ".y";
		break;
	case SWIZZLE_Z:
		return symbolName + ".z";
		break;
	case SWIZZLE_W:
		return symbolName + ".w";
		break;
	case SWIZZLE_XY:
		return symbolName + ".xy";
		break;
	case SWIZZLE_XZ:
		return symbolName + ".xz";
		break;
	case SWIZZLE_ZZ:
		return symbolName + ".zz";
		break;
	case SWIZZLE_XYZ:
		return symbolName + ".xyz";
		break;
	case SWIZZLE_XYZW:
		return symbolName + ".xyzw";
		break;
	default:
		assert(false);
		return symbolName;
		break;
	}
}
