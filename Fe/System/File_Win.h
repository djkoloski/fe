#pragma once

#include <Fe/Core/Core.h>

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Container/FeString.h>

class WindowsFile
{
public:
	static feString readAllText(feStringView path);
	static void readAllBytes(feStringView path, feByte *&bytes);
	static void readAllBytes(feStringView path, feByte *&bytes, feUWord &bytesRead);

	static void writeAllText(feStringView path, feStringView text);
	static void writeAllBytes(feStringView path, feUByte *&bytes, feUWord size);

	static feUWord getSizeInBytes(feStringView path);

	static void create(feStringView path);
	static feBool exists(feStringView path);
private:
	static const feUInt k_BufferSizeInBytes = 1024 * 32;
};

#endif