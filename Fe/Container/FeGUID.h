#pragma once

#include <Fe/Core/Core.h>
#include <Fe/Container/FeString.h>

class feGUID
{
private:
	feUByte _bytes[16];

public:
	feGUID();
	feGUID(feStringView str);
	feGUID(
		feUByte x1, feUByte x2, feUByte x3, feUByte x4,
		feUByte x5, feUByte x6, feUByte x7, feUByte x8,
		feUByte x9, feUByte x10, feUByte x11, feUByte x12,
		feUByte x13, feUByte x14, feUByte x15, feUByte x16);

	feUByte *getBytes();
	const feUByte *getBytes() const;
	feString toString() const;
	void fromString(feStringView str);

	feBool operator==(const feGUID &other) const;
	feBool operator!=(const feGUID &other) const;
};