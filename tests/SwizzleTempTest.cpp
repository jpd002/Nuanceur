#include "SwizzleTempTest.h"
#include "nuanceur/Builder.h"

void CSwizzleTempTest::Run()
{
	using namespace Nuanceur;

	auto b = CShaderBuilder();

	float col = 127.f / 255.f;

	{
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));
		auto tempValue1 = CFloat4Lvalue(b.CreateTemporary());
		auto tempValue2 = CFloat4Lvalue(b.CreateTemporary());
		auto tempValue2Z = CFloatLvalue(tempValue2.symbol, SWIZZLE_Z);

		tempValue1 = NewFloat4(b, 0, 0, col, 0);
		tempValue2 = NewFloat4(b, 1, 1, 1, 1);

		tempValue2Z = tempValue1->z();
		
		outputColor = NewFloat4(tempValue2->xyz(), tempValue1->z());
	}

	Submit(b, CVector4(1, 1, col, col));
}
