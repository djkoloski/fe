#pragma once

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
	Type _type;
	feVector<BuildCommand> _buildCommands;
	feVector<const Module *> _modules;
	
public:
	Project(feStringView name);

	const feString &getName() const;
	Type getType() const;
	void setType(Type type);
	const feVector<BuildCommand> &getBuildCommands() const;
	BuildCommand &addBuildCommand();
	const feVector<const Module *> &getModules() const;
	void addModule(const Module *module);
};