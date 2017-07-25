#pragma once

#include <Fe/Core/FeFeature.h>

#define FE_COMPILER_TYPE_MSVC	FE_ENUM_TYPE
#define FE_IS_COMPILER(x)		FE_IS_ENUM(FE_COMPILER, x)

// MSVC
#if defined(_MSC_VER)

#define FE_COMPILER_MSVC		FE_ENUM_VALUE

// Unsupported compiler
#else

#error Unsupported compiler

#endif