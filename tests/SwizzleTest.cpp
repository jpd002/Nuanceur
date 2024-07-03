#include "SwizzleTest.h"
#include "nuanceur/Builder.h"

void CSwizzleTest::Run()
{
	using namespace Nuanceur;

	auto b = CShaderBuilder();

	{
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));
		auto outputColorXY = CFloat2Lvalue(outputColor->xy().symbol);

		outputColor = NewFloat4(b, 1, 1, 1, 1);
		outputColorXY = NewFloat2(b, 0, 0);
	}

	Submit(b, CVector4(0, 0, 1, 1));
}
