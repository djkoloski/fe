#pragma once

#include <Fe/Core/Core.h>

#if FE_IS_TARGET(WINDOWS)
#include <Fe/System/File_Win.h>
using File = WindowsFile;
#endif