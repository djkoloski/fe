#include <Makey/Rule.h>

Rule::Rule(feStringView name) :
	_name(name),
	_isGenerator(false),
	_restat(false)
{}
const feString &Rule::getName() const
{
	return _name;
}
const feString &Rule::getCommand() const
{
	return _command;
}
void Rule::setCommand(feStringView command)
{
	_command = command;
}
const feString &Rule::getDescription() const
{
	return _description;
}
void Rule::setDescription(feStringView description)
{
	_description = description;
}
const feString &Rule::getDeps() const
{
	return _deps;
}
void Rule::setDeps(feStringView deps)
{
	_deps = deps;
}
feBool Rule::getIsGenerator() const
{
	return _isGenerator;
}
void Rule::setIsGenerator(feBool isGenerator)
{
	_isGenerator = isGenerator;
}
feBool Rule::getRestat() const
{
	return _restat;
}
void Rule::setRestat(feBool restat)
{
	_restat = restat;
}


#if FE_USING(FE_FEATURE_UNIT_TESTS)

#include <Fe/Core/FeUnitTest.h>

TEST_CASE("Testing Catch", "[test]") {
	REQUIRE((1 + 1) == 2);
}

#endif