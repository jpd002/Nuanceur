#include "StorageArrayTest.h"
#include "nuanceur/Builder.h"

void CStorageArrayTest::Run()
{
	using namespace Nuanceur;

	auto b = CShaderBuilder();

	{
		auto storageFloatArray = CArrayFloat4Value(b.CreateStorageArrayFloat4("floatValues"));
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));

		outputColor = Load(storageFloatArray, NewInt(b, 2));
	}

	auto result = CVector4(55.f / 255.f, 244.f / 255.f, 255.f / 255.f, 0);

	CTestContext testContext;
	for(int i = 0; i < 4; i++)
	{
		testContext.storageBuffer.push_back({0, 0, 0, 0});
	}
	testContext.storageBuffer[2] = result;
	testContext.expectedValue = result;

	Submit(b, testContext);
}
