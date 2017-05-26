#include <Makey/Module.h>

Module::Module(feStringView name) :
	_name(name)
{}
const feString &Module::getName() const
{
	return _name;
}
const feVector<feString> &Module::getIncludes() const
{
	return _includes;
}
void Module::addInclude(feStringView include)
{
	_includes.push_back(include);
}
const feVector<feString> &Module::getLibs() const
{
	return _libs;
}
void Module::addLib(feStringView lib)
{
	_libs.push_back(lib);
}
const feVector<feString> &Module::getSharedLibs() const
{
	return _sharedLibs;
}
void Module::addSharedLib(feStringView sharedLib)
{
	_sharedLibs.push_back(sharedLib);
}
const feVector<const Project *> &Module::getDependencies() const
{
	return _dependencies;
}
void Module::addDependency(const Project *dependency)
{
	_dependencies.push_back(dependency);
}