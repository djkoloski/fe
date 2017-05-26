#pragma once

#include <Fe/Core/FeConfig.h>

// Enabled
#if FE_USING(FE_FEATURE_DEBUG_BREAK)

#include <Fe/Core/FeCompiler.h>

// MSVC
#if FE_IS_COMPILER(MSVC)

#define FE_DEBUG_BREAK() __debugbreak()

#endif

// Disabled
#else

#define FE_DEBUG_BREAK() ((void)0)

#endif