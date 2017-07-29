#pragma once

#include <Fe/Core/Core.h>

#include <string>

using feString = std::string;
using feStringView = const std::string &;

class feStringUtil
{
public:
	template <typename ...Rest>
	static feString join(feStringView first, const Rest &...rest);
	template <typename Iterator>
	static feString joinRange(Iterator begin, Iterator end);
	template <typename ...Rest>
	static feString joinWrapped(feStringView prefix, feStringView suffix, feStringView separator, feStringView first, const Rest &...rest);
	template <typename Iterator>
	static feString joinRangeWrapped(feStringView prefix, feStringView suffix, feStringView separator, Iterator begin, Iterator end);
	static feString append(feStringView str, feStringView append, feChar delimiter = ' ');
	static feString toLower(feStringView str);
	static feString toUpper(feStringView str);
	static feString replace(feStringView source, feStringView pattern, feStringView replacement);

private:
	template <typename ...Rest>
	static feUWord _joinLength(feStringView first, const Rest &...rest);
	static feUWord _joinLength(feStringView first);
	template <typename ...Rest>
	static void _join(feString &out, feStringView first, const Rest &...rest);
	static void _join(feString &out, feStringView first);
	template <typename ...Rest>
	static void _joinWrapped(feString &out, feStringView prefix, feStringView suffix, feStringView separator, feStringView first, const Rest &...rest);
	static void _joinWrapped(feString &out, feStringView prefix, feStringView suffix, feStringView separator, feStringView first);
};

#include <Fe/Container/FeString.inl>