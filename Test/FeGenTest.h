#pragma once

#include <Fe/Core/Core.h>
#include <Fe/Container/FePointer.h>

#include <string>
#include <unordered_set>

class HelperTestClass : public feObject
{
private:
	feInt _a;
	feInt _b;
};

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
	feUnique<TestClass> _unique;
	feShared<TestClass> _shared;
	feWeak<TestClass> _weak;
	Meta(Get, Set)
	HelperTestClass _helper;
};