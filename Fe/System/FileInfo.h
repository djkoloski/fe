#pragma once

#include <Fe/Core/Core.h>
#include <Fe/Container/FeString.h>

enum class FileType
{
	File,
	Directory,
};

class FileInfo
{
private:
	feString _path;
	feString _name;
	FileType _type;

public:
	FileInfo(feStringView path, feStringView name, FileType type);

	const feString &getPath() const;
	const feString &getName() const;
	FileType getType() const;
};