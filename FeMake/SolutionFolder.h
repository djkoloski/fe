#pragma once

#include <Fe/Container/FeGUID.h>
#include <Fe/Container/FeString.h>
#include <Fe/Container/FeVector.h>

class SolutionFolder
{
private:
	feString _name;
	feGUID _visualStudioGUID;
	feVector<feString> _paths;

public:
	SolutionFolder(feStringView name, const feGUID &visualStudioGUID);

	const feString &getName() const;
	const feGUID &getVisualStudioGUID() const;
	void addPath(feStringView path);
	const feVector<feString> getPaths() const;
};