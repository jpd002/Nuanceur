#include "BasicTest.h"
#include "nuanceur/Builder.h"

void CBasicTest::Run()
{
	using namespace Nuanceur;

	auto b = CShaderBuilder();

	{
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));
		outputColor = NewFloat4(b, 1, 1, 1, 1);
	}

	Submit(b, CVector4(1, 1, 1, 1));
}
