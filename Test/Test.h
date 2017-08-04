#pragma once

#include <Fe/Core/Core.h>

#include <string>
#include <unordered_set>

class TestClass : public feObject
{
private:
	Meta(Get, Set)
	feBool _a;
protected:
	Meta(Get, Set)
	feInt _b;
public:
	Meta(Get, Set)
	feUWord _c;
};