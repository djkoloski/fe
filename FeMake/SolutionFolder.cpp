#include <FeMake/SolutionFolder.h>

SolutionFolder::SolutionFolder(feStringView name, const feGUID &visualStudioGUID) :
	_name(name),
	_visualStudioGUID(visualStudioGUID)
{}
const feString &SolutionFolder::getName() const
{
	return _name;
}
const feGUID &SolutionFolder::getVisualStudioGUID() const
{
	return _visualStudioGUID;
}
void SolutionFolder::addPath(feStringView path)
{
	_paths.push_back(path);
}
const feVector<feString> SolutionFolder::getPaths() const
{
	return _paths;
}