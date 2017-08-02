#include <Makey/Project.h>

Project::Project(feStringView name, const feGUID &visualStudioGUID) :
	_name(name),
	_visualStudioGUID(visualStudioGUID),
	_codegenEnabled(true)
{}
const feString &Project::getName() const
{
	return _name;
}
const feGUID &Project::getVisualStudioGUID() const
{
	return _visualStudioGUID;
}
Project::Type Project::getType() const
{
	return _type;
}
void Project::setType(Type type)
{
	_type = type;
}
const feVector<BuildCommand> &Project::getBuildCommands() const
{
	return _buildCommands;
}
BuildCommand &Project::addBuildCommand()
{
	_buildCommands.emplace_back();
	return _buildCommands.back();
}
BuildCommand &Project::getBuildAlias()
{
	return _buildAlias;
}
const BuildCommand &Project::getBuildAlias() const
{
	return _buildAlias;
}
const feVector<const Module *> &Project::getModules() const
{
	return _modules;
}
void Project::addModule(const Module *module)
{
	_modules.push_back(module);
}
feBool Project::getCodegenEnabled() const
{
	return _codegenEnabled;
}
void Project::setCodegenEnabled(feBool codegenEnabled)
{
	_codegenEnabled = codegenEnabled;
}
void Project::addHeaderFilePath(feStringView headerFilePath)
{
	_headerFilePaths.push_back(headerFilePath);
}
const feVector<feString> &Project::getHeaderFilePaths() const
{
	return _headerFilePaths;
}
void Project::addSourceFilePath(feStringView sourceFilePath)
{
	_sourceFilePaths.push_back(sourceFilePath);
}
const feVector<feString> &Project::getSourceFilePaths() const
{
	return _sourceFilePaths;
}
void Project::addOtherFilePath(feStringView otherFilePath)
{
	_otherFilePaths.push_back(otherFilePath);
}
const feVector<feString> &Project::getOtherFilePaths() const
{
	return _otherFilePaths;
}
void Project::collectDependentModules(feHashTable<feString, const Module *> &modules) const
{
	for (const auto *module : _modules)
	{
		if (modules.count(module->getName()) == 0)
		{
			modules[module->getName()] = module;
		}
		for (const auto *dependency : module->getDependencies())
		{
			dependency->collectDependentModules(modules);
		}
	}
}