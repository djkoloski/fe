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
	static feStatus create(feStringView path, feBool recurse = true);
};

#include <Fe/System/Directory_Win.inl>

#endif