#include <Test/Test.h>

TestClass::TestClass(feBool a)
{}
feBool TestClass::operator==(const TestClass &other) const
{
	return true;
}