#pragma once

#include <Fe/Core/Core.h>

#if FE_IS_TARGET(WINDOWS)
#include <Fe/System/Directory_Win.h>
using Directory = WindowsDirectory;
#endif