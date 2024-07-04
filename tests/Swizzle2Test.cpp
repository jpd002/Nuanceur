#include "Swizzle2Test.h"
#include "nuanceur/Builder.h"

void CSwizzle2Test::Run()
{
	using namespace Nuanceur;

	auto b = CShaderBuilder();

	float colY = 127.f / 255.f;
	float colZ = 63.f / 255.f;

	{
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));
		auto outputColorYZ = CFloat2Lvalue(outputColor.symbol, SWIZZLE_YZ);

		outputColor = NewFloat4(b, 1, 1, 1, 0);
		outputColorYZ = NewFloat2(b, colY, colZ);
	}

	Submit(b, CVector4(1, colY, colZ, 0));
}
