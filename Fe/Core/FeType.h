#pragma once

#include <Fe/Core/FePlatform.h>

#include <cstdint>

enum feStatus
{
	kFailure = 0,
	kSuccess = 1,
};

using feBool = bool;
using feChar = char;
using feRawString = const feChar *;
using feFloat = float;
using feDouble = double;
using Null = decltype(nullptr);
#define null nullptr

using feByte = int8_t;
using feUByte = uint8_t;
using feShort = int16_t;
using feUShort = uint16_t;
using feInt = int32_t;
using feUInt = uint32_t;

// 32 bit
#if FE_IS_ARCH(32BIT)

using feWord = feInt;
using feUWord = feUInt;

// 64 bit
#elif FE_IS_ARCH(64BIT)

using feLong = int64_t;
using feULong = uint64_t;
using feWord = feLong;
using feUWord = feULong;

#endif