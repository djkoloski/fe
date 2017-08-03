#pragma once

#include <Fe/Core/Core.h>

#include <string>
#include <unordered_set>

// Broken right now:
// - Enums
// - Type aliases
// - Macro definitions

class feObject
{};

class TestClassBase
{};

class TestClass : public feObject, public TestClassBase
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

	static const feUWord k_constant = 20;
	feUByte array[k_constant];

	TestClass() = default;
	TestClass(const TestClass &other) = default;
	TestClass(TestClass &&other) = delete;
	TestClass(feBool a);

	virtual int getClassMeta() const = 0;

	feBool operator==(const TestClass &other) const;

	TestClass &operator=(const TestClass &other);
	TestClass &operator=(TestClass &&other);

	static std::string getSpelling(int cursor);
	static int getAccessSpecifier(int cursor);
	static std::string getAccessSpecifierSpelling(int accessSpecifier);
	static std::unordered_set<std::string> getAttributes(int cursor);

	template <typename T>
	void foo();

	// Broken right now:
	// - Defaulting operator=
	// - Default argument values
	// - Deleted builtin functions
	// - Anything templated
};