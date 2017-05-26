#pragma once

#include <Fe/Core/FeFeature.h>

#define FE_TARGET_TYPE_WINDOWS	FE_ENUM_TYPE
#define FE_IS_TARGET(x)			FE_IS_ENUM(FE_TARGET, x)

#define FE_ARCH_TYPE_32BIT		FE_ENUM_TYPE
#define FE_ARCH_TYPE_64BIT		FE_ENUM_TYPE
#define FE_IS_ARCH(x)			FE_IS_ENUM(FE_ARCH, x)

// Windows x86
#if defined(PLATFORM_WIN_X86)

#define FE_TARGET_WINDOWS		FE_ENUM_VALUE
#define FE_ARCH_32BIT			FE_ENUM_VALUE

// Windows x64
#elif defined(PLATFORM_WIN_X64)

#define FE_TARGET_WINDOWS		FE_ENUM_VALUE
#define FE_ARCH_64BIT			FE_ENUM_VALUE

// Unsupported platform
#else

#error Unsupported platform

#endif