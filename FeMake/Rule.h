#pragma once

#include <Fe/Container/FeString.h>

class Rule
{
private:
	feString _name;
	feString _command;
	feString _description;
	feString _deps;
	feString _depfile;
	feBool _isGenerator;
	feBool _restat;

public:
	Rule(feStringView name);

	const feString &getName() const;
	const feString &getCommand() const;
	void setCommand(feStringView command);
	const feString &getDescription() const;
	void setDescription(feStringView description);
	const feString &getDeps() const;
	void setDeps(feStringView deps);
	const feString &getDepfile() const;
	void setDepfile(feStringView depfile);
	feBool getIsGenerator() const;
	void setIsGenerator(feBool isGenerator);
	feBool getRestat() const;
	void setRestat(feBool restat);
};