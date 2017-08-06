#include <Fe/System/File_Win.h>

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Core/FeWindows.h>

feBool WindowsFile::exists(feStringView path)
{
	auto attributes = GetFileAttributes(path.c_str());
	return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

#endif