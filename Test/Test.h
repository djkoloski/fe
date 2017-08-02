#pragma once

#include <Fe/Core/Core.h>

class TestClass
{
private:
	Meta(Get, Set)
	feBool _a;
	Meta(Get, Set)
	feInt _b;
	Meta(Get, Set)
	feUWord _c;

};