#include <Fe/Algorithm/Hash.h>

#include <Fe/Algorithm/Internal/sha1.h>

SHA1Hash::SHA1Hash()
{
	for (auto i = 0; i < k_byteCount; ++i)
	{
		bytes[i] = 0;
	}
}
feBool SHA1Hash::operator==(const SHA1Hash &other) const
{
	for (auto i = 0; i < k_byteCount; ++i)
	{
		if (bytes[i] != other.bytes[i])
		{
			return false;
		}
	}
	return true;
}
feBool SHA1Hash::operator!=(const SHA1Hash &other) const
{
	for (auto i = 0; i < k_byteCount; ++i)
	{
		if (bytes[i] != other.bytes[i])
		{
			return true;
		}
	}
	return false;
}

SHA1Hash Hash::SHA1(feStringView str)
{
	auto result = SHA1Hash();
	::SHA1((char *)result.bytes, str.c_str(), str.length());
	return result;
}