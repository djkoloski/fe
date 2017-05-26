#include <Fe/System/FileInfo.h>

FileInfo::FileInfo(feStringView path, feStringView name, FileType type) :
	_path(path),
	_name(name),
	_type(type)
{}
const feString &FileInfo::getPath() const
{
	return _path;
}
const feString &FileInfo::getName() const
{
	return _name;
}
FileType FileInfo::getType() const
{
	return _type;
}