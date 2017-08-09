#include <Fe/Container/FeString.h>

#include <cctype>

feUWord feStringUtil::_joinLength(feStringView first)
{
	return first.length();
}
void feStringUtil::_join(feString &out, feStringView first)
{
	out += first;
}
void feStringUtil::_joinWrapped(feString &out, feStringView prefix, feStringView suffix, feStringView separator, feStringView first)
{
	out += prefix;
	out += first;
	out += suffix;
}
feString feStringUtil::append(feStringView str, feStringView append, feChar delimiter)
{
	auto result = feString(str);
	if (str.length())
	{
		result += delimiter;
	}
	result += append;
	return result;
}
feString feStringUtil::toLower(feStringView str)
{
	auto result = feString();
	result.resize(str.length());

	for (auto i = 0; i < str.length(); ++i)
	{
		result[i] = tolower(str[i]);
	}

	return result;
}
feString feStringUtil::toUpper(feStringView str)
{
	auto result = feString();
	result.resize(str.length());

	for (auto i = 0; i < str.length(); ++i)
	{
		result[i] = toupper(str[i]);
	}

	return result;
}
feString feStringUtil::replace(feStringView source, feStringView pattern, feStringView replacement)
{
	auto result = feString(source);
	for (auto i = feString::size_type(0); (i = result.find(pattern, i)) != feString::npos;)
	{
		result.replace(i, pattern.length(), replacement);
		i += replacement.length();
	}
	return result;
}
feBool feStringUtil::isEqual(feStringView first, feStringView second)
{
	return !strcmp(first.c_str(), second.c_str());
}