#include <Makey/Project.h>

Project::Project(feStringView name) :
	_name(name)
{}
const feString &Project::getName() const
{
	return _name;
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
const feVector<const Module *> &Project::getModules() const
{
	return _modules;
}
void Project::addModule(const Module *module)
{
	_modules.push_back(module);
}