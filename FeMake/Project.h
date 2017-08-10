#pragma once

#include <Fe/Container/FeGUID.h>
#include <Fe/Container/FeString.h>
#include <Fe/Container/FeVector.h>
#include <Fe/Container/FeHashTable.h>
#include <FeMake/BuildCommand.h>
#include <FeMake/Module.h>

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
	feBool _codegenEnabled;
	feVector<feString> _headerFilePaths;
	feVector<feString> _sourceFilePaths;
	feVector<feString> _otherFilePaths;
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
	const feVector<const Module *> &getModules() const;
	void addModule(const Module *module);
	feBool getCodegenEnabled() const;
	void setCodegenEnabled(feBool codegenEnabled);
	void addHeaderFilePath(feStringView headerFilePath);
	const feVector<feString> &getHeaderFilePaths() const;
	void addSourceFilePath(feStringView sourceFilePath);
	const feVector<feString> &getSourceFilePaths() const;
	void addOtherFilePath(feStringView otherFilePath);
	const feVector<feString> &getOtherFilePaths() const;
	void collectDependentModules(feHashTable<feString, const Module *> &modules) const;
	const feString &getVisualStudioDebuggerDefaultArguments() const;
	void setVisualStudioDebuggerDefaultArguments(feStringView arguments);
};