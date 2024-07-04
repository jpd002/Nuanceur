#include "Swizzle1Test.h"
#include "nuanceur/Builder.h"

void CSwizzle1Test::Run()
{
	using namespace Nuanceur;

	auto b = CShaderBuilder();

	float colZ = 63.f / 255.f;

	{
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));
		auto outputColorZ = CFloatLvalue(outputColor.symbol, SWIZZLE_Z);

		auto tempValue = CFloat4Lvalue(b.CreateConstant(0.1, 0.2, colZ, 0.4));

		outputColor = NewFloat4(b, 0, 1, 1, 0);
		outputColorZ = tempValue->z();
	}

	Submit(b, CVector4(0, 1, colZ, 0));
}
