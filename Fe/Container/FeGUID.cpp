#include <Fe/Container/FeGUID.h>

namespace
{

const feChar k_byteToUppercaseChar[16] = {
	'0', '1', '2', '3',
	'4', '5', '6', '7',
	'8', '9', 'A', 'B',
	'C', 'D', 'E', 'F',
};
feChar upperToUppercaseHex(feUByte byte)
{
	return k_byteToUppercaseChar[byte >> 4];
}
feChar lowerToUppercaseHex(feUByte byte)
{
	return k_byteToUppercaseChar[byte & 0x0f];
}
feUByte hexToLower(feChar lower)
{
	if (lower >= '0' && lower <= '9')
	{
		return lower - '0';
	}
	else if (lower >= 'A' && lower <= 'F')
	{
		return lower - 'A';
	}
	else
	{
		return lower - 'a';
	}
}
feUByte hexToUpper(feChar upper)
{
	return hexToLower(upper) << 4;
}
feUByte hexToByte(feChar upper, feChar lower)
{
	return hexToUpper(upper) | hexToLower(lower);
}

}

feGUID::feGUID()
{
	_bytes[0] = 0;
	_bytes[1] = 0;
	_bytes[2] = 0;
	_bytes[3] = 0;
	_bytes[4] = 0;
	_bytes[5] = 0;
	_bytes[6] = 0;
	_bytes[7] = 0;
	_bytes[8] = 0;
	_bytes[9] = 0;
	_bytes[10] = 0;
	_bytes[11] = 0;
	_bytes[12] = 0;
	_bytes[13] = 0;
	_bytes[14] = 0;
	_bytes[15] = 0;
}
feGUID::feGUID(feStringView str)
{
	fromString(str);
}
feGUID::feGUID(
	feUByte x1, feUByte x2, feUByte x3, feUByte x4,
	feUByte x5, feUByte x6, feUByte x7, feUByte x8,
	feUByte x9, feUByte x10, feUByte x11, feUByte x12,
	feUByte x13, feUByte x14, feUByte x15, feUByte x16)
{
	_bytes[0] = x1;
	_bytes[1] = x2;
	_bytes[2] = x3;
	_bytes[3] = x4;
	_bytes[4] = x5;
	_bytes[5] = x6;
	_bytes[6] = x7;
	_bytes[7] = x8;
	_bytes[8] = x9;
	_bytes[9] = x10;
	_bytes[10] = x11;
	_bytes[11] = x12;
	_bytes[12] = x13;
	_bytes[13] = x14;
	_bytes[14] = x15;
	_bytes[15] = x16;
}
feUByte *feGUID::getBytes()
{
	return _bytes;
}
const feUByte *feGUID::getBytes() const
{
	return _bytes;
}
feString feGUID::toString() const
{
	auto str = feString(36, '0');

	str[0] = upperToUppercaseHex(_bytes[0]);
	str[1] = lowerToUppercaseHex(_bytes[0]);
	str[2] = upperToUppercaseHex(_bytes[1]);
	str[3] = lowerToUppercaseHex(_bytes[1]);
	str[4] = upperToUppercaseHex(_bytes[2]);
	str[5] = lowerToUppercaseHex(_bytes[2]);
	str[6] = upperToUppercaseHex(_bytes[3]);
	str[7] = lowerToUppercaseHex(_bytes[3]);
	
	str[8] = '-';

	str[9] = upperToUppercaseHex(_bytes[4]);
	str[10] = lowerToUppercaseHex(_bytes[4]);
	str[11] = upperToUppercaseHex(_bytes[5]);
	str[12] = lowerToUppercaseHex(_bytes[5]);

	str[13] = '-';

	str[14] = upperToUppercaseHex(_bytes[6]);
	str[15] = lowerToUppercaseHex(_bytes[6]);
	str[16] = upperToUppercaseHex(_bytes[7]);
	str[17] = lowerToUppercaseHex(_bytes[7]);

	str[18] = '-';

	str[19] = upperToUppercaseHex(_bytes[8]);
	str[20] = lowerToUppercaseHex(_bytes[8]);
	str[21] = upperToUppercaseHex(_bytes[9]);
	str[22] = lowerToUppercaseHex(_bytes[9]);

	str[23] = '-';

	str[24] = upperToUppercaseHex(_bytes[10]);
	str[25] = lowerToUppercaseHex(_bytes[10]);
	str[26] = upperToUppercaseHex(_bytes[11]);
	str[27] = lowerToUppercaseHex(_bytes[11]);
	str[28] = upperToUppercaseHex(_bytes[12]);
	str[29] = lowerToUppercaseHex(_bytes[12]);
	str[30] = upperToUppercaseHex(_bytes[13]);
	str[31] = lowerToUppercaseHex(_bytes[13]);
	str[32] = upperToUppercaseHex(_bytes[14]);
	str[33] = lowerToUppercaseHex(_bytes[14]);
	str[34] = upperToUppercaseHex(_bytes[15]);
	str[35] = lowerToUppercaseHex(_bytes[15]);

	return str;
}
void feGUID::fromString(feStringView str)
{
	_bytes[0] = hexToByte(str[0], str[1]);
	_bytes[1] = hexToByte(str[2], str[3]);
	_bytes[2] = hexToByte(str[4], str[5]);
	_bytes[3] = hexToByte(str[6], str[7]);

	_bytes[4] = hexToByte(str[9], str[10]);
	_bytes[5] = hexToByte(str[11], str[12]);

	_bytes[6] = hexToByte(str[14], str[15]);
	_bytes[7] = hexToByte(str[16], str[17]);

	_bytes[8] = hexToByte(str[19], str[20]);
	_bytes[9] = hexToByte(str[21], str[22]);

	_bytes[10] = hexToByte(str[24], str[25]);
	_bytes[11] = hexToByte(str[26], str[27]);
	_bytes[12] = hexToByte(str[28], str[29]);
	_bytes[13] = hexToByte(str[30], str[31]);
	_bytes[14] = hexToByte(str[32], str[33]);
	_bytes[15] = hexToByte(str[34], str[35]);
}
feBool feGUID::operator==(const feGUID &other) const
{
	for (auto i = 0; i < 16; ++i)
	{
		if (_bytes[i] != other._bytes[i])
		{
			return false;
		}
	}
	return true;
}
feBool feGUID::operator!=(const feGUID &other) const
{
	for (auto i = 0; i < 16; ++i)
	{
		if (_bytes[i] != other._bytes[i])
		{
			return true;
		}
	}
	return false;
}