#include "LoopTest.h"
#include "nuanceur/Builder.h"

void CLoopTest::Run()
{
	constexpr int loopCount = 6;

	using namespace Nuanceur;

	auto b = CShaderBuilder();

	{
		auto outputColor = CFloat4Lvalue(b.CreateOutput(Nuanceur::SEMANTIC_SYSTEM_COLOR));
		auto increment = CFloat4Lvalue(b.CreateUniformFloat4("increment"));
		auto counter = CFloatLvalue(b.CreateVariableFloat("counter"));
		auto accumulator = CFloat4Lvalue(b.CreateVariableFloat("accumulator"));

		counter = NewFloat(b, 0);
		accumulator = NewFloat4(b, 0, 0, 0, 0);

		BeginLoop(b);
		{
			BeginIf(b, counter->x() == NewFloat(b, 1));
			{
				counter = counter->x() + NewFloat(b, 1);
				LoopContinue(b);
			}
			EndIf(b);
			BeginIf(b, counter->x() >= NewFloat(b, loopCount));
			{
				LoopBreak(b);
			}
			EndIf(b);
			counter = counter->x() + NewFloat(b, 1);
			accumulator = accumulator->xyzw() + increment->xyzw();
		}
		EndLoop(b);
		outputColor = accumulator->xyzw();
	}

	auto increment = CVector4(1.f / 255.f, 2.f / 255.f, 3.f / 255.f, 4.f / 255.f);
	auto result = increment * (loopCount - 1);

	CTestContext testContext;
	testContext.uniformBuffer.push_back(increment);
	testContext.expectedValue = result;

	Submit(b, testContext);
}
