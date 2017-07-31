#pragma once

#include <Fe/Core/FeConfig.h>
#include <Fe/Core/FeType.h>

// Enabled
#if FE_USING(FE_FEATURE_PRINT)

void fePrint(feRawString fmt, ...);

#define FE_PRINT(fmt, ...) fePrint(fmt, __VA_ARGS__)

// Disabled
#else

#define FE_PRINT(fmt, ...) ((void)0)

#endif

// Logging
#define FE_LOG(level, fmt, ...) FE_PRINT(#level ": " fmt "\n", __VA_ARGS__)
#define FE_LOG_WARNING(fmt, ...) FE_LOG(WARNING, fmt, __VA_ARGS__)
#define FE_LOG_ERROR(fmt, ...) FE_LOG(ERROR, fmt, __VA_ARGS__)