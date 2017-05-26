#include <Makey/Solution.h>

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
const feHashTable<feString, feUnique<Project>> &Solution::getProjects() const
{
	return _projects;
}
Project *Solution::addProject(feStringView name)
{
	_projects[name] = feMakeUnique<Project>(name);
	return _projects[name].get();
}
const Project *Solution::getProject(feStringView name) const
{
	auto i = _projects.find(name);
	if (i == _projects.end())
	{
		return null;
	}
	return i->second.get();
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