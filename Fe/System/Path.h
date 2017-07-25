#pragma once

#include <Fe/Core/Core.h>
#include <Fe/Container/FeString.h>

class Path
{
public:
	template <typename ...Rest>
	static feString join(feStringView first, const Rest &...rest);
	static feString baseName(feStringView path);
	static feString dirName(feStringView path);
	static feString extension(feStringView path);
	static feString removeExtension(feStringView path);
	static feString addExtension(feStringView path, feStringView extension);
	static feString removePrefix(feStringView path, feStringView prefix);
	static feString fileName(feStringView path);

private:
	static feUWord _baseNameLength(feStringView path);
	static feUWord _extensionLength(feStringView path);
};

#include <Fe/System/Path.inl>