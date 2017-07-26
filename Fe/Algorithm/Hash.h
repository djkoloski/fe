#pragma once

#include <Fe/Core/Core.h>
#include <Fe/Container/FeString.h>

class SHA1Hash
{
public:
	static const feUWord k_byteCount = 20;
	feUByte bytes[k_byteCount];

	SHA1Hash();

	feBool operator==(const SHA1Hash &other) const;
	feBool operator!=(const SHA1Hash &other) const;
};

class Hash
{
public:
	static SHA1Hash SHA1(feStringView str);
};