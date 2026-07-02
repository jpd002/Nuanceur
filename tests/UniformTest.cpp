#include "UniformTest.h"
#include "nuanceur/Builder.h"

void CUniformTest::Run()
{
	using namespace Nuanceur;

	auto b = CShaderBuilder();

	{
		auto uniformFloatValue = CFloat4Lvalue(b.CreateUniformFloat4("floatValue"));
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));

		outputColor = uniformFloatValue->xyzw();
	}

	auto result = CVector4(55.f / 255.f, 244.f / 255.f, 255.f / 255.f, 0);

	CTestContext testContext;
	testContext.uniformBuffer.push_back(result);
	testContext.expectedValue = result;

	Submit(b, testContext);
}
