#pragma once

#include <vector>
#include "math/Vector4.h"
#include "Types.h"

namespace Nuanceur
{
	class CShaderBuilder;
}

class CTest
{
public:
	virtual ~CTest() = default;
	virtual void Run() = 0;

protected:
	void Submit(const Nuanceur::CShaderBuilder&, const CVector4&);

private:
	std::vector<uint32> GenerateCode(const Nuanceur::CShaderBuilder&);
};
