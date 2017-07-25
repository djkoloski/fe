#pragma once

#include <Fe/System/Directory_Win.h>

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Core/FeWindows.h>
#include <Fe/System/FileInfo.h>
#include <Fe/System/Path.h>

template <typename T>
void WindowsDirectory::iterate(feStringView path, T eval, feBool recurse)
{
	auto findData = WIN32_FIND_DATA();
	auto hFind = FindFirstFile(Path::join(path, "*").c_str(), &findData);

	do
	{
		if (hFind == INVALID_HANDLE_VALUE)
		{
			break;
		}

		auto fileName = feString(findData.cFileName);
		if (fileName == "." || fileName == "..")
		{
			continue;
		}

		auto fileType = FileType::File;
		if (findData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		{
			fileType = FileType::Directory;
		}

		auto fileInfo = FileInfo(Path::join(path, fileName), fileName, fileType);

		eval((const FileInfo &)fileInfo);

		switch (fileInfo.getType())
		{
		case FileType::File:
			break;
		case FileType::Directory:
			if (recurse)
			{
				iterate(fileInfo.getPath(), eval, recurse);
			}
			break;
		default:
			break;
		}
	}
	while (FindNextFile(hFind, &findData));

	FindClose(hFind);
	hFind = INVALID_HANDLE_VALUE;
}

#endif