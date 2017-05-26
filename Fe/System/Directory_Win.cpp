#include <Fe/System/Directory_Win.h>

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Core/FeWindows.h>

feString WindowsDirectory::currentWorkingDirectory()
{
	feString result;
	auto length = GetCurrentDirectory(0, null);
	result.resize(length - 1);
	GetCurrentDirectory(length, &result[0]);
	return result;
}

#endif