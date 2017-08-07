#pragma once

#include <Fe/Core/Core.h>

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Container/FeString.h>

class WindowsFile
{
public:
	static feBool exists(feStringView path);
};

#endif