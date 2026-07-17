#include <functional>
#include "BasicTest.h"
#include "StorageArrayTest.h"
#include "Swizzle1Test.h"
#include "Swizzle2Test.h"
#include "SwizzleTempTest.h"
#include "UniformTest.h"
#include "UniformArrayTest.h"

typedef std::function<CTest*()> TestFactoryFunction;

// clang-format off
static const TestFactoryFunction s_factories[] =
{
	[]() { return new CBasicTest(); },
	[]() { return new CStorageArrayTest(); },
	[]() { return new CSwizzle1Test(); },
	[]() { return new CSwizzle2Test(); },
	[]() { return new CSwizzleTempTest(); },
	[]() { return new CUniformTest(); },
	[]() { return new CUniformArrayTest(); },
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
