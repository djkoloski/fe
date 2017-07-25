#pragma once

#include <Fe/Container/FeGUID.h>
#include <Fe/Container/FeString.h>
#include <Fe/Container/FeVector.h>
#include <Makey/BuildCommand.h>
#include <Makey/Module.h>

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
	feVector<const Module *> _modules;

public:
	Project(feStringView name, const feGUID &visualStudioGUID);

	const feString &getName() const;
	const feGUID &getVisualStudioGUID() const;
	Type getType() const;
	void setType(Type type);
	const feVector<BuildCommand> &getBuildCommands() const;
	BuildCommand &addBuildCommand();
	const feVector<const Module *> &getModules() const;
	void addModule(const Module *module);
};