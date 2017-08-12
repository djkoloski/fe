#pragma once

#include <Fe/Container/FeGUID.h>
#include <Fe/Container/FeString.h>
#include <Fe/Container/FeVector.h>
#include <Fe/Container/FeHashTable.h>
#include <FeMake/BuildCommand.h>
#include <FeMake/Module.h>

class Solution;

class Project
{
public:
	enum class Type
	{
		Executable,
		Library
	};

private:
	feString _name;
	feGUID _visualStudioGUID;
	Type _type;
	feVector<BuildCommand> _buildCommands;
	BuildCommand _buildAlias;
	feVector<const Module *> _modules;
	feVector<feString> _headerFilePaths;
	feVector<feString> _inlineFilePaths;
	feVector<feString> _sourceFilePaths;
	feVector<feString> _otherFilePaths;
	feBool _codegenEnabled;
	feString _visualStudioDebuggerDefaultArguments;

public:
	Project(feStringView name, const feGUID &visualStudioGUID);

	const feString &getName() const;
	const feGUID &getVisualStudioGUID() const;
	Type getType() const;
	void setType(Type type);
	const feVector<BuildCommand> &getBuildCommands() const;
	BuildCommand &addBuildCommand();
	BuildCommand &getBuildAlias();
	const BuildCommand &getBuildAlias() const;
	void addModule(const Module *module);
	const feVector<const Module *> &getModules() const;
	void addHeaderFilePath(feStringView headerFilePath);
	const feVector<feString> &getHeaderFilePaths() const;
	void addInlineFilePath(feStringView otherFilePath);
	const feVector<feString> &getInlineFilePaths() const;
	void addSourceFilePath(feStringView sourceFilePath);
	const feVector<feString> &getSourceFilePaths() const;
	void addOtherFilePath(feStringView otherFilePath);
	const feVector<feString> &getOtherFilePaths() const;
	void setCodegenEnabled(feBool codegenEnabled);
	feBool getCodegenEnabled() const;
	void collectDependentModules(feHashTable<feString, const Module *> &modules) const;
	const feString &getVisualStudioDebuggerDefaultArguments() const;
	void setVisualStudioDebuggerDefaultArguments(feStringView arguments);
	void collectFiles();
	void makeBuildRules(const Solution &solution);

private:
	void makeCodegenBuildRules(feStringView path, const Solution &solution, feString &buildHeaders, feString &libObjects);
	feString makeHeaderCopyBuildRule(feStringView path, const Solution &solution);
	feString makeSourceBuildRule(feStringView path, const Solution &solution);
	void makeBuildHeadersBuildRule(feStringView buildHeaders, const Solution &solution);
	feString makeLibraryDependencyBuildRules(const Solution &solution);
	feString makeLibraryBuildRule(feStringView objects, const Solution &solution);
	feString makeExecutableBuildRule(feStringView objects, feStringView libraryDependencies, const Solution &solution, feBool wholeArchive);
	void makeUnitTestBuildRule(feStringView executablePath, const Solution &solution);
};