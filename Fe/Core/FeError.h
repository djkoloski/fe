#pragma once

#include <Fe/Core/FeConfig.h>
#include <Fe/Core/FeUtility.h>

// Enabled
#if FE_USING(FE_FEATURE_RUNTIME_ERRORS)

#include <Fe/Core/FePrint.h>
#include <Fe/Core/FeDebugBreak.h>

#define FE_ERROR(msg, ...) _FE_ERROR(msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define _FE_ERROR(msg, file, line, ...) \
do \
{ \
	FE_LOG_ERROR(msg, ##__VA_ARGS__); \
	FE_DEBUG_BREAK(); \
	feAbort(); \
} while (0)

#define FE_ASSERT(cond, msg, ...) _FE_ASSERT(cond, msg, __FILE__, __LINE__, ##__VA_ARGS__)
#define _FE_ASSERT(cond, msg, file, line, ...) \
do \
{ \
	if (!(cond)) \
	{ \
		_FE_ERROR("%s:%i Assertion failed (" #cond "): " msg "\n", file, line, ##__VA_ARGS__); \
	} \
} while (0)
#define FE_ASSERT_ONLY(x) x

// Disabled
#else

#define FE_ERROR(msg, ...) ((void)0)

#define FE_ASSERT(cond, msg, ...) ((void)0)
#define FE_ASSERT_ONLY(code)

#endif

#define FE_ERROR_SWITCH_VALUE() FE_ERROR("Invalid value in switch statement")