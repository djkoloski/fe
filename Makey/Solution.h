#pragma once

#include <Fe/Container/FePointer.h>
#include <Fe/Container/FeString.h> 
#include <Fe/Container/FeVector.h>
#include <Fe/Container/FeHashTable.h>
#include <Makey/Module.h>
#include <Makey/Project.h>
#include <Makey/Rule.h>
#include <Makey/Settings.h>

class Solution
{
private:
	feString _solutionDir;
	Settings _settings;
	feHashTable<feString, feUnique<Rule>> _rules;
	feHashTable<feString, feUnique<Project>> _projects;
	feHashTable<feString, feUnique<Module>> _modules;
	
public:
	const feString &getSolutionDir() const;
	void setSolutionDir(feStringView solutionDir);
	const Settings &getSettings() const;
	void setSettings(const Settings &settings);
	const feHashTable<feString, feUnique<Rule>> &getRules() const;
	Rule *addRule(feStringView name);
	const Rule *getRule(feStringView name) const;
	const feHashTable<feString, feUnique<Project>> &getProjects() const;
	Project *addProject(feStringView name);
	const Project *getProject(feStringView name) const;
	const feHashTable<feString, feUnique<Module>> &getModules() const;
	Module *addModule(feStringView name);
	const Module *getModule(feStringView name) const;
};