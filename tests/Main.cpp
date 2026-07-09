#include <functional>
#include "BasicTest.h"
#include "LoopTest.h"
#include "Swizzle1Test.h"
#include "Swizzle2Test.h"
#include "SwizzleTempTest.h"
#include "UniformTest.h"

typedef std::function<CTest*()> TestFactoryFunction;

// clang-format off
static const TestFactoryFunction s_factories[] =
{
	[]() { return new CBasicTest(); },
	[]() { return new CLoopTest(); },
	[]() { return new CSwizzle1Test(); },
	[]() { return new CSwizzle2Test(); },
	[]() { return new CSwizzleTempTest(); },
	[]() { return new CUniformTest(); },
};
// clang-format on

int main(int argc, char** argv)
{
	for(const auto& factory : s_factories)
	{
		auto test = factory();
		test->Run();
		delete test;
	}
}
