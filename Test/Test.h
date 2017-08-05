#pragma once

#include <Fe/Core/Core.h>

#include <string>
#include <unordered_set>

class TestClass : public feObject
{
private:
	Meta(Get, Set)
	feBool _bool;
	Meta(Get, Set)
	feChar _char;
	Meta(Get, Set)
	feFloat _float;
	Meta(Get, Set)
	feDouble _double;
	Meta(Get, Set)
	feByte _byte;
	Meta(Get, Set)
	feUByte _ubyte;
	Meta(Get, Set)
	feShort _short;
	Meta(Get, Set)
	feUShort _ushort;
	Meta(Get, Set)
	feInt _int;
	Meta(Get, Set)
	feUInt _uint;
#if FE_IS_ARCH(64BIT)
	Meta(Get, Set)
	feLong _long;
	Meta(Get, Set)
	feULong _ulong;
#endif
};