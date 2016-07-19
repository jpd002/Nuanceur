#include "nuanceur/generators/GlslShaderGenerator.h"
#include "string_format.h"

using namespace Nuanceur;

CGlslShaderGenerator::CGlslShaderGenerator(const CShaderBuilder& shaderBuilder, SHADER_TYPE shaderType)
: m_shaderBuilder(shaderBuilder)
, m_shaderType(shaderType)
{

}

std::string CGlslShaderGenerator::Generate(const CShaderBuilder& shaderBuilder, SHADER_TYPE shaderType)
{
	CGlslShaderGenerator generator(shaderBuilder, shaderType);
	return generator.Generate();
}

std::string CGlslShaderGenerator::Generate() const
{
	std::string result;

	if(m_shaderType == SHADER_TYPE_FRAGMENT)
	{
		result += "precision mediump float;\r\n";
	}
	
	result += GenerateInputs();
	result += GenerateOutputs();
	result += GenerateUniforms();
	result += GenerateSamplers();

	result += "void main()\r\n";
	result += "{\r\n";

	//Write all temps
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEMPORARY) continue;
		auto temporaryValue = m_shaderBuilder.GetTemporaryValue(symbol);
		result += string_format("\tvec4 %s = vec4(%f, %f, %f, %f);\r\n",
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
			result += string_format("\t%s = %s * %s;\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str());
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
			result += string_format("\t%s = mix(%s, %s, %s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str(),
				PrintSymbolRef(src3Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_NEWVECTOR2:
			result += string_format("\t%s = vec2(%s, %s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str(),
				PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_NEWVECTOR4:
			switch(statement.GetSourceCount())
			{
			case 2:
				result += string_format("\t%s = vec4(%s, %s);\r\n",
					PrintSymbolRef(dstRef).c_str(),
					PrintSymbolRef(src1Ref).c_str(),
					PrintSymbolRef(src2Ref).c_str());
				break;
			case 3:
				result += string_format("\t%s = vec4(%s, %s, %s);\r\n",
					PrintSymbolRef(dstRef).c_str(),
					PrintSymbolRef(src1Ref).c_str(),
					PrintSymbolRef(src2Ref).c_str(),
					PrintSymbolRef(src3Ref).c_str());
				break;
			case 4:
				result += string_format("\t%s = vec4(%s, %s, %s, %s);\r\n",
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
			result += string_format("\t%s = clamp(%s, 0.0, 1.0);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				PrintSymbolRef(src1Ref).c_str());
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
			assert(src1Ref.symbol.location == CShaderBuilder::SYMBOL_LOCATION_TEXTURE);
			result += string_format("\t%s = texture2D(c_sampler%d, %s);\r\n",
				PrintSymbolRef(dstRef).c_str(),
				src1Ref.symbol.index,
				PrintSymbolRef(src2Ref).c_str());
			break;
		default:
			assert(0);
			break;
		}
	}
	
	result += "}\r\n";

	return result;
}

std::string CGlslShaderGenerator::GenerateInputs() const
{
	std::string result;
	const char* inputTag = (m_shaderType == SHADER_TYPE_VERTEX) ? "attribute" : "varying";
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_INPUT) continue;
		auto semantic = m_shaderBuilder.GetInputSemantic(symbol);
		if(semantic.type == SEMANTIC_SYSTEM_POSITION) continue;
		if(semantic.type == SEMANTIC_SYSTEM_COLOR) continue;
		result += string_format("%s %s %s;\r\n",
			inputTag, MakeTypeName(symbol.type).c_str(),
			MakeLocalSymbolName(symbol).c_str());
	}
	return result;
}

std::string CGlslShaderGenerator::GenerateOutputs() const
{
	std::string result;
	const char* inputTag = (m_shaderType == SHADER_TYPE_VERTEX) ? "varying" : "(invalid)";
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_OUTPUT) continue;
		auto semantic = m_shaderBuilder.GetOutputSemantic(symbol);
		if(semantic.type == SEMANTIC_SYSTEM_POSITION) continue;
		if(semantic.type == SEMANTIC_SYSTEM_COLOR) continue;
		result += string_format("%s %s %s;\r\n",
			inputTag, MakeTypeName(symbol.type).c_str(),
			MakeLocalSymbolName(symbol).c_str());
	}
	return result;
}

std::string CGlslShaderGenerator::GenerateUniforms() const
{
	std::string result;
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_UNIFORM) continue;
		auto constantType = (symbol.type == CShaderBuilder::SYMBOL_TYPE_MATRIX) ? "mat4" : "vec4";
		result += string_format("uniform %s %s;\r\n",
			constantType, MakeLocalSymbolName(symbol).c_str());
	}
	return result;
}

std::string CGlslShaderGenerator::GenerateSamplers() const
{
	std::string result;
	//Generate samplers/textures
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_TEXTURE) continue;
		result += string_format("uniform sampler2D c_sampler%d;\r\n", symbol.index);
	}
	return result;
}

std::string CGlslShaderGenerator::MakeSymbolName(const CShaderBuilder::SYMBOL& sym) const
{
	switch(sym.location)
	{
	case CShaderBuilder::SYMBOL_LOCATION_TEMPORARY:
		return string_format("t%d", sym.index);
		break;
	case CShaderBuilder::SYMBOL_LOCATION_INPUT:
		return MakeLocalSymbolName(sym);
		break;
	case CShaderBuilder::SYMBOL_LOCATION_OUTPUT:
		return MakeLocalSymbolName(sym);
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

std::string CGlslShaderGenerator::MakeLocalSymbolName(const CShaderBuilder::SYMBOL& sym) const
{
	switch(sym.location)
	{
	case CShaderBuilder::SYMBOL_LOCATION_INPUT:
		{
			auto semantic = m_shaderBuilder.GetInputSemantic(sym);
			const char* prefix = (m_shaderType == SHADER_TYPE_VERTEX) ? "a" : "v";
			return string_format("%s_%s", prefix, MakeSemanticName(semantic).c_str());
		}
		break;
	case CShaderBuilder::SYMBOL_LOCATION_OUTPUT:
		{
			auto semantic = m_shaderBuilder.GetOutputSemantic(sym);
			if(semantic.type == SEMANTIC_SYSTEM_POSITION)
			{
				return "gl_Position";
			}
			else if(semantic.type == SEMANTIC_SYSTEM_COLOR)
			{
				return "gl_FragColor";
			}
			else
			{
				const char* prefix = (m_shaderType == SHADER_TYPE_VERTEX) ? "v" : "invalid";
				return string_format("%s_%s", prefix, MakeSemanticName(semantic).c_str());
			}
		}
		break;
	case CShaderBuilder::SYMBOL_LOCATION_UNIFORM:
		return m_shaderBuilder.GetUniformName(sym);
	default:
		assert(false);
		return "unknown";
	}
}

std::string CGlslShaderGenerator::MakeSemanticName(CShaderBuilder::SEMANTIC_INFO semantic)
{
	switch(semantic.type)
	{
	case SEMANTIC_POSITION:
		return string_format("position%d", semantic.index);
	case SEMANTIC_TEXCOORD:
		return string_format("texCoord%d", semantic.index);
	default:
		assert(false);
		return "";
	}
}

std::string CGlslShaderGenerator::MakeTypeName(CShaderBuilder::SYMBOL_TYPE type)
{
	switch(type)
	{
	case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		return "vec4";
	case CShaderBuilder::SYMBOL_TYPE_MATRIX:
		return "mat4";
	case CShaderBuilder::SYMBOL_TYPE_TEXTURE2D:
		return "Texture2D";
	default:
		assert(false);
		return "";
	}
}

std::string CGlslShaderGenerator::PrintSymbolRef(const CShaderBuilder::SYMBOLREF& ref) const
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
	case SWIZZLE_W:
		return symbolName + ".w";
		break;
	case SWIZZLE_XY:
		return symbolName + ".xy";
		break;
	case SWIZZLE_XZ:
		return symbolName + ".xz";
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
