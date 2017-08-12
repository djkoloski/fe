#pragma once

#include <Fe/Container/FePointer.h>
#include <Fe/Container/FeString.h> 
#include <Fe/Container/FeVector.h>
#include <Fe/Container/FeHashTable.h>
#include <FeMake/Module.h>
#include <FeMake/Project.h>
#include <FeMake/Rule.h>
#include <FeMake/Settings.h>
#include <FeMake/SolutionFolder.h>

class Solution
{
private:
	feString _name;
	feGUID _visualStudioGUID;
	feString _solutionDir;
	Settings _settings;
	feHashTable<feString, feUnique<Rule>> _rules;
	feVector<feUnique<Project>> _projects;
	feHashTable<feString, feUnique<Module>> _modules;
	feGUID _visualStudioSolutionFoldersGUID;
	feVector<feUnique<SolutionFolder>> _visualStudioSolutionFolders;

public:
	Solution(feStringView name, const feGUID &visualStudioGUID);

	const feString &getName() const;
	const feGUID &getVisualStudioGUID() const;
	const feString &getSolutionDir() const;
	void setSolutionDir(feStringView solutionDir);
	const Settings &getSettings() const;
	void setSettings(const Settings &settings);
	const feHashTable<feString, feUnique<Rule>> &getRules() const;
	Rule *addRule(feStringView name);
	const Rule *getRule(feStringView name) const;
	const feVector<feUnique<Project>> &getProjects() const;
	Project *addProject(feStringView name, const feGUID visualStudioGUID);
	const Project *getProject(feStringView name) const;
	const feHashTable<feString, feUnique<Module>> &getModules() const;
	Module *addModule(feStringView name);
	const Module *getModule(feStringView name) const;
	void setVisualStudioSolutionFoldersGUID(const feGUID &visualStudioSolutionFoldersGUID);
	const feGUID &getVisualStudioSolutionFoldersGUID() const;
	SolutionFolder *addVisualStudioSolutionFolder(feStringView name, const feGUID &visualStudioSolutionFolderGUID);
	const feVector<feUnique<SolutionFolder>> &getVisualStudioSolutionFolders() const;
};