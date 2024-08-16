#include "nuanceur/generators/GlslShaderGenerator.h"
#include "string_format.h"

using namespace Nuanceur;

CGlslShaderGenerator::CGlslShaderGenerator(const CShaderBuilder& shaderBuilder, SHADER_TYPE shaderType, uint32 glslVersion)
    : m_shaderBuilder(shaderBuilder)
    , m_shaderType(shaderType)
    , m_glslVersion(glslVersion)
{
}

std::string CGlslShaderGenerator::Generate(const CShaderBuilder& shaderBuilder, SHADER_TYPE shaderType, uint32 glslVersion)
{
	CGlslShaderGenerator generator(shaderBuilder, shaderType, glslVersion);
	return generator.Generate();
}

std::string CGlslShaderGenerator::Generate() const
{
	std::string result;

	if(m_glslVersion != 0)
	{
		result += string_format("#version %d\r\n", m_glslVersion);
	}

	if(m_shaderType == SHADER_TYPE_FRAGMENT)
	{
		result += "precision mediump float;\r\n";
	}
	else if(m_shaderType == SHADER_TYPE_COMPUTE)
	{
		uint32 localSizeX = m_shaderBuilder.GetMetadata(CShaderBuilder::METADATA_LOCALSIZE_X, 1);
		uint32 localSizeY = m_shaderBuilder.GetMetadata(CShaderBuilder::METADATA_LOCALSIZE_Y, 1);
		uint32 localSizeZ = m_shaderBuilder.GetMetadata(CShaderBuilder::METADATA_LOCALSIZE_Z, 1);
		result += string_format("layout(local_size_x = %d, local_size_y = %d, local_size_z = %d) in;\r\n",
			localSizeX, localSizeY, localSizeZ);
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
		switch(symbol.type)
		{
		default:
			assert(false);
			[[fallthrough]];
		case CShaderBuilder::SYMBOL_TYPE_FLOAT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValue(symbol);
			result += string_format("\t%s %s = vec4(%f, %f, %f, %f);\r\n",
			                        MakeTypeName(symbol.type).c_str(), MakeSymbolName(symbol).c_str(),
			                        temporaryValue.x, temporaryValue.y, temporaryValue.z, temporaryValue.w);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_INT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueInt(symbol);
			result += string_format("\t%s %s = ivec4(%d, %d, %d, %d);\r\n",
			                        MakeTypeName(symbol.type).c_str(), MakeSymbolName(symbol).c_str(),
			                        temporaryValue.x, temporaryValue.y, temporaryValue.z, temporaryValue.w);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_UINT4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueInt(symbol);
			result += string_format("\t%s %s = uvec4(%u, %u, %u, %u);\r\n",
			                        MakeTypeName(symbol.type).c_str(), MakeSymbolName(symbol).c_str(),
			                        temporaryValue.x, temporaryValue.y, temporaryValue.z, temporaryValue.w);
		}
		break;
		case CShaderBuilder::SYMBOL_TYPE_BOOL4:
		{
			auto temporaryValue = m_shaderBuilder.GetTemporaryValueBool(symbol);
			result += string_format("\t%s %s = bvec4(%u, %u, %u, %u);\r\n",
			                        MakeTypeName(symbol.type).c_str(), MakeSymbolName(symbol).c_str(),
			                        temporaryValue.x, temporaryValue.y, temporaryValue.z, temporaryValue.w);
		}
		break;
		}
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
		case CShaderBuilder::STATEMENT_OP_MODULO:
			result += string_format("\t%s = %s %% %s;\r\n",
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
		case CShaderBuilder::STATEMENT_OP_TOFLOAT:
			result += EmitConversion({"float", "vec2", "vec3", "vec4"}, dstRef, src1Ref);
			break;
		case CShaderBuilder::STATEMENT_OP_TOINT:
			result += EmitConversion({"int", "ivec2", "ivec3", "ivec4"}, dstRef, src1Ref);
			break;
		case CShaderBuilder::STATEMENT_OP_LOAD:
			result += string_format("\t%s = %s[%s];\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        MakeSymbolName(src1Ref.symbol).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_STORE:
			result += string_format("\t%s[%s] = %s;\r\n",
			                        MakeSymbolName(src1Ref.symbol).c_str(),
			                        PrintSymbolRef(src2Ref).c_str(),
			                        PrintSymbolRef(src3Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_RSHIFT:
			result += string_format("\t%s = %s >> %s;\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_AND:
			result += string_format("\t%s = %s & %s;\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_COMPARE_GE:
			result += string_format("\t%s = %s >= %s;\r\n",
			                        PrintSymbolRef(dstRef).c_str(),
			                        PrintSymbolRef(src1Ref).c_str(),
			                        PrintSymbolRef(src2Ref).c_str());
			break;
		case CShaderBuilder::STATEMENT_OP_IF_BEGIN:
			result += string_format("\tif(%s)\r\n", PrintSymbolRef(src1Ref).c_str());
			result += "\t{\r\n";
			break;
		case CShaderBuilder::STATEMENT_OP_IF_END:
			result += "\t}\r\n";
			break;
		case CShaderBuilder::STATEMENT_OP_RETURN:
			result += "\treturn;\r\n";
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
	if(m_glslVersion >= 130)
	{
		inputTag = "in";
	}
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_INPUT) continue;
		auto semantic = m_shaderBuilder.GetInputSemantic(symbol);
		if(semantic.type == SEMANTIC_SYSTEM_POSITION) continue;
		if(semantic.type == SEMANTIC_SYSTEM_COLOR) continue;
		if(semantic.type == SEMANTIC_SYSTEM_GIID) continue;
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
	if(m_glslVersion >= 130)
	{
		inputTag = "out";
	}
	for(const auto& symbol : m_shaderBuilder.GetSymbols())
	{
		if(symbol.location != CShaderBuilder::SYMBOL_LOCATION_OUTPUT) continue;
		auto semantic = m_shaderBuilder.GetOutputSemantic(symbol);
		if(semantic.type == SEMANTIC_SYSTEM_POSITION) continue;
		if((semantic.type == SEMANTIC_SYSTEM_COLOR) && (m_glslVersion <= 420)) continue;
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
		if(symbol.unit == UNIFORM_UNIT_PUSHCONSTANT)
		{
			result += string_format("uniform %s %s;\r\n",
			                        MakeTypeName(symbol.type).c_str(), MakeLocalSymbolName(symbol).c_str());
		}
		else
		{
			assert(symbol.type == CShaderBuilder::SYMBOL_TYPE_ARRAYUINT);
			result += string_format("layout(std430, binding = %d) buffer uniforms_%d\r\n",
			                        symbol.unit, symbol.unit);
			result += string_format("{\r\n\tuint %s[];\r\n};\r\n", MakeLocalSymbolName(symbol).c_str());
		}
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
		if(semantic.type == SEMANTIC_SYSTEM_GIID)
		{
			return "ivec3(gl_GlobalInvocationID)";
		}
		else
		{
			const char* prefix = (m_shaderType == SHADER_TYPE_VERTEX) ? "a" : "v";
			return string_format("%s_%s", prefix, MakeSemanticName(semantic).c_str());
		}
	}
	break;
	case CShaderBuilder::SYMBOL_LOCATION_OUTPUT:
	{
		auto semantic = m_shaderBuilder.GetOutputSemantic(sym);
		if(semantic.type == SEMANTIC_SYSTEM_POSITION)
		{
			return "gl_Position";
		}
		else if((semantic.type == SEMANTIC_SYSTEM_COLOR) && (m_glslVersion <= 420))
		{
			return "gl_FragColor";
		}
		else
		{
			const char* prefix = (m_shaderType == SHADER_TYPE_VERTEX) ? "v" : "o";
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
	case SEMANTIC_SYSTEM_COLOR:
		return string_format("color%d", semantic.index);
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
	case CShaderBuilder::SYMBOL_TYPE_INT4:
		return "ivec4";
	case CShaderBuilder::SYMBOL_TYPE_UINT4:
		return "uvec4";
	case CShaderBuilder::SYMBOL_TYPE_BOOL4:
		return "bvec4";
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
	static const char elemChars[4] = {'x', 'y', 'z', 'w'};
	std::string result = symbolName + ".";
	auto elemCount = GetSwizzleElementCount(ref.swizzle);
	for(int i = 0; i < elemCount; i++)
	{
		int elem = GetSwizzleElement(ref.swizzle, i);
		assert(elem < 4);
		result += elemChars[elem];
	}
	return result;
}

std::string CGlslShaderGenerator::EmitConversion(const std::array<const char*, 4>& dstTypes, const CShaderBuilder::SYMBOLREF& dstRef, const CShaderBuilder::SYMBOLREF& src1Ref) const
{
	assert(GetSwizzleElementCount(dstRef.swizzle) == GetSwizzleElementCount(src1Ref.swizzle));
	auto dstType = dstTypes[GetSwizzleElementCount(dstRef.swizzle) - 1];
	return string_format("\t%s = %s(%s);\r\n",
	                     PrintSymbolRef(dstRef).c_str(),
	                     dstType,
	                     PrintSymbolRef(src1Ref).c_str());
}
