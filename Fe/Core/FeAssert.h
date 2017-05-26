#pragma once

#include <Fe/Core/FeConfig.h>
#include <Fe/Core/FeUtility.h>

// Enabled
#if FE_USING(FE_FEATURE_ASSERT)

#include <Fe/Core/FePrint.h>
#include <Fe/Core/FeDebugBreak.h>

#define FE_ASSERT(cond, msg, ...) _FE_ASSERT(cond, msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define _FE_ASSERT(cond, msg, file, line, ...) \
do \
{ \
	if (!(cond)) \
	{ \
		FE_LOG_ERROR("%s:%i Assertion failed (" #cond "): " msg "\n", file, line, ##__VA_ARGS__); \
		FE_DEBUG_BREAK(); \
		feAbort(); \
	} \
} while (0)
#define FE_ASSERT_ONLY(x) x

// Disabled
#else

#define FE_ASSERT(cond, msg, ...) ((void)0)
#define FE_ASSERT_ONLY(code)

#endif