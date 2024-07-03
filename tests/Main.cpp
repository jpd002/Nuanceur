#include <functional>
#include "BasicTest.h"
#include "SwizzleTest.h"

typedef std::function<CTest*()> TestFactoryFunction;

// clang-format off
static const TestFactoryFunction s_factories[] =
{
	[]() { return new CBasicTest(); },
	[]() { return new CSwizzleTest(); },
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
