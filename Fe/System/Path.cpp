#include <Fe/System/Path.h>

feString Path::baseName(feStringView path)
{
	auto i = _baseNameLength(path);
	return path.substr(path.length() - i, i);
}
feString Path::dirName(feStringView path)
{
	auto i = _baseNameLength(path);
	return path.substr(0, path.length() - i - 1);
}
feString Path::extension(feStringView path)
{
	auto i = _extensionLength(path);
	return path.substr(path.length() - i, i);
}
feString Path::removeExtension(feStringView path)
{
	auto i = _extensionLength(path);
	return path.substr(0, path.length() - i);
}
feString Path::addExtension(feStringView path, feStringView extension)
{
	return path + extension;
}
feString Path::removePrefix(feStringView path, feStringView prefix)
{
	return path.substr(prefix.length() + 1);
}
feString Path::fileName(feStringView path)
{
	return baseName(removeExtension(path));
}

feUWord Path::_baseNameLength(feStringView path)
{
	for (auto i = path.length() - 1; ; --i)
	{
#if FE_IS_TARGET(WINDOWS)
		if (path[i] == '\\')
#else
		if (path[i] == '/' && (i == 0 || path[i - 1] != '\\'))
#endif
		{
			return path.length() - i - 1;
		}
		if (i == 0)
		{
			break;
		}
	}
	
	return path.length();
}
feUWord Path::_extensionLength(feStringView path)
{
	auto lastIndex = path.length();
	for (auto i = path.length() - 1; ; --i)
	{
#if FE_IS_TARGET(WINDOWS)
		if (path[i] == '.')
#else
		if (path[i] == '.' && (i == 0 || path[i - 1] == '\\'))
#endif
		{
			lastIndex = i;
		}
#if FE_IS_TARGET(WINDOWS)
		if (path[i] == '\\')
#else
		if (path[i] == '/' && (i == 0 || path[i - 1] != '\\'))
#endif
		{
			break;
		}
		if (i == 0)
		{
			break;
		}
	}
	
	return path.length() - lastIndex;
}