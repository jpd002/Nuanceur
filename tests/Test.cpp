#include "Test.h"
#include <vkrunner/vkrunner.h>
#include <string>
#include "nuanceur/Builder.h"
#include "nuanceur/generators/SpirvShaderGenerator.h"
#include "MemStream.h"
#include "string_format.h"

void CTest::Submit(const Nuanceur::CShaderBuilder& shaderBuilder, const CTestContext& testContext)
{
	std::string testString;

	testString += "[require]\r\n";
	if(!testContext.storageBuffer.empty())
	{
		testString += "fragmentStoresAndAtomics\r\n";
	}

	testString += "[vertex shader passthrough]\r\n";
	testString += "[fragment shader binary]\r\n";

	auto shader = GenerateCode(shaderBuilder);

	static constexpr uint32 lineSize = 16;
	uint32 lines = (shader.size() + lineSize - 1) / lineSize;
	for(int i = 0; i < lines; i++)
	{
		for(int j = 0; j < lineSize; j++)
		{
			uint32 itemIndex = (i * lineSize) + j;
			if(itemIndex == shader.size()) break;
			testString += string_format("%x ", shader[itemIndex]);
		}
		testString += "\r\n";
	}

	testString += "[test]\r\n";
	testString += "clear\r\n";
	for(int i = 0; i < testContext.uniformBuffer.size(); i++)
	{
		const auto& uboData = testContext.uniformBuffer[i];
		testString += string_format("ubo 0 subdata vec4 %d %f %f %f %f\r\n",
		                            i * 16, uboData.x, uboData.y, uboData.z, uboData.w);
	}
	for(int i = 0; i < testContext.storageBuffer.size(); i++)
	{
		const auto& uboData = testContext.storageBuffer[i];
		testString += string_format("ssbo 0 subdata vec4 %d %f %f %f %f\r\n",
		                            i * 16, uboData.x, uboData.y, uboData.z, uboData.w);
	}
	testString += "draw rect -1 -1 2 2\r\n";
	testString += "\r\n";

	testString += string_format("probe all rgba %f %f %f %f\r\n",
	                            testContext.expectedValue.x,
	                            testContext.expectedValue.y,
	                            testContext.expectedValue.z,
	                            testContext.expectedValue.w);

	/* Create a source representing the file */
	struct vr_source* source = vr_source_from_string(testString.c_str());

	struct vr_config* config = vr_config_new();
	struct vr_executor* executor = vr_executor_new(config);
	enum vr_result result = vr_executor_execute(executor, source);
	vr_executor_free(executor);
	vr_config_free(config);

	vr_source_free(source);

	printf("Test status is: %s\n",
	       vr_result_to_string(result));

	assert(result == VR_RESULT_PASS);
}

void CTest::Submit(const Nuanceur::CShaderBuilder& shaderBuilder, const CVector4& expectedValue)
{
	CTestContext context;
	context.expectedValue = expectedValue;
	Submit(shaderBuilder, context);
}

std::vector<uint32> CTest::GenerateCode(const Nuanceur::CShaderBuilder& shaderBuilder)
{
	Framework::CMemStream shaderStream;
	Nuanceur::CSpirvShaderGenerator::Generate(shaderStream, shaderBuilder, Nuanceur::CSpirvShaderGenerator::SHADER_TYPE_FRAGMENT);
	return std::vector<uint32>(reinterpret_cast<uint32*>(shaderStream.GetBuffer()), reinterpret_cast<uint32*>(shaderStream.GetBuffer() + shaderStream.GetSize()));
}
