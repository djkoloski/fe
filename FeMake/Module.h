#pragma once

#include <Fe/Core/Core.h>
#include <Fe/Container/FeString.h>
#include <Fe/Container/FeVector.h>

class Project;

class Module
{
private:
	feString _name;
	feVector<feString> _includes;
	feVector<feString> _libs;
	feVector<feString> _sharedLibs;
	feVector<const Project *> _dependencies;

public:
	Module(feStringView name);

	const feString &getName() const;
	const feVector<feString> &getIncludes() const;
	void addInclude(feStringView include);
	const feVector<feString> &getLibs() const;
	void addLib(feStringView lib);
	const feVector<feString> &getSharedLibs() const;
	void addSharedLib(feStringView sharedLib);
	const feVector<const Project *> &getDependencies() const;
	void addDependency(const Project *dependency);
};