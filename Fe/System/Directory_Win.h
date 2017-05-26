#pragma once

#include <Fe/Core/Core.h>

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Container/FeString.h>

class WindowsDirectory
{
public:
	static feString currentWorkingDirectory();
	template <typename T>
	static void iterate(feStringView path, T eval, feBool recurse = false);
};

#include <Fe/System/Directory_Win.inl>

#endif