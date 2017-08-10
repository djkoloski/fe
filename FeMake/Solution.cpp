#include <FeMake/Solution.h>

Solution::Solution(feStringView name, const feGUID &visualStudioGUID) :
	_name(name),
	_visualStudioGUID(visualStudioGUID)
{}
const feString &Solution::getName() const
{
	return _name;
}
const feGUID &Solution::getVisualStudioGUID() const
{
	return _visualStudioGUID;
}
const feString &Solution::getSolutionDir() const
{
	return _solutionDir;
}
void Solution::setSolutionDir(feStringView solutionDir)
{
	_solutionDir = solutionDir;
}
const Settings &Solution::getSettings() const
{
	return _settings;
}
void Solution::setSettings(const Settings &settings)
{
	_settings = settings;
}
const feHashTable<feString, feUnique<Rule>> &Solution::getRules() const
{
	return _rules;
}
Rule *Solution::addRule(feStringView name)
{
	_rules[name] = feMakeUnique<Rule>(name);
	return _rules[name].get();
}
const Rule *Solution::getRule(feStringView name) const
{
	auto i = _rules.find(name);
	if (i == _rules.end())
	{
		return null;
	}
	return i->second.get();
}
const feVector<feUnique<Project>> &Solution::getProjects() const
{
	return _projects;
}
Project *Solution::addProject(feStringView name, const feGUID visualStudioGUID)
{
	_projects.push_back(feMakeUnique<Project>(name, visualStudioGUID));
	return _projects.back().get();
}
const Project *Solution::getProject(feStringView name) const
{
	for (const auto &project : _projects)
	{
		if (project->getName() == name)
		{
			return project.get();
		}
	}
	return null;
}
const feHashTable<feString, feUnique<Module>> &Solution::getModules() const
{
	return _modules;
}
Module *Solution::addModule(feStringView name)
{
	_modules[name] = feMakeUnique<Module>(name);
	return _modules[name].get();
}
const Module *Solution::getModule(feStringView name) const
{
	auto i = _modules.find(name);
	if (i == _modules.end())
	{
		return null;
	}
	return i->second.get();
}
void Solution::setVisualStudioSolutionFoldersGUID(const feGUID &visualStudioSolutionFoldersGUID)
{
	_visualStudioSolutionFoldersGUID = visualStudioSolutionFoldersGUID;
}
const feGUID &Solution::getVisualStudioSolutionFoldersGUID() const
{
	return _visualStudioSolutionFoldersGUID;
}
SolutionFolder *Solution::addVisualStudioSolutionFolder(feStringView name, const feGUID &visualStudioSolutionFolderGUID)
{
	_visualStudioSolutionFolders.push_back(feMakeUnique<SolutionFolder>(name, visualStudioSolutionFolderGUID));
	return _visualStudioSolutionFolders.back().get();
}
const feVector<feUnique<SolutionFolder>> &Solution::getVisualStudioSolutionFolders() const
{
	return _visualStudioSolutionFolders;
}