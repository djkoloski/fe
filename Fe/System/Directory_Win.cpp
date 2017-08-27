#include <Fe/System/Directory_Win.h>

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Core/FeWindows.h>

feString WindowsDirectory::currentWorkingDirectory()
{
	auto result = feString();
	auto length = GetCurrentDirectory(0, null);
	result.resize(length - 1);
	GetCurrentDirectory(length, &result[0]);
	return result;
}
feStatus WindowsDirectory::create(feStringView path, feBool recurse)
{
	DWORD pathType = GetFileAttributes(path.c_str());
	if (pathType == INVALID_FILE_ATTRIBUTES)
	{
		if (recurse)
		{
			auto parent = Path::dirName(path);
			if (parent != "")
			{
				if (create(parent, true) == kFailure)
				{
					return kFailure;
				}
			}
		}

		if (!CreateDirectory(path.c_str(), null) && GetLastError() != ERROR_ALREADY_EXISTS)
		{
			return kFailure;
		}
	}
	else if (!(pathType & FILE_ATTRIBUTE_DIRECTORY))
	{
		return kFailure;
	}

	return kSuccess;
}

#endif