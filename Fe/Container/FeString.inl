#pragma once

#include <Fe/Container/FeString.h>

template <typename ...Rest>
feString feStringUtil::join(feStringView first, const Rest &...rest)
{
	auto length = _joinLength(first, rest...);
	feString out;
	out.reserve(length);
	_join(out, first, rest...);
	return out;
}
template <typename Iterator>
feString feStringUtil::joinRange(Iterator begin, Iterator end)
{
	auto length = 0;
	for (Iterator i = begin; i != end; ++i)
	{
		length += feStringView(*i).length();
	}
	feString out;
	out.reserve(length);
	for (; begin != end; ++begin)
	{
		out += *begin;
	}
	return out;
}
template <typename ...Rest>
feString feStringUtil::joinWrapped(feStringView prefix, feStringView suffix, feStringView separator, feStringView first, const Rest &...rest)
{
	auto argCount = 1 + sizeof...(rest);
	auto length = _joinLength(first, rest...) + argCount * (prefix.length() + suffix.length() + separator.length()) - separator.length();
	feString out;
	out.reserve(length);
	_joinWrapped(out, prefix, suffix, separator, first, rest...);
	return out;
}
template <typename Iterator>
feString feStringUtil::joinRangeWrapped(feStringView prefix, feStringView suffix, feStringView separator, Iterator begin, Iterator end)
{
	auto length = feUWord(0);
	
	auto i = begin;
	if (i != end)
	{
		length += prefix.length() + feStringView(*i).length() + suffix.length();
		++i;
	}
	for (; i != end; ++i)
	{
		length += separator.length() + prefix.length() + feStringView(*i).length() + suffix.length();
	}
	
	feString out;
	out.reserve(length);
	
	if (begin != end)
	{
		out += prefix + *begin + suffix;
		++begin;
	}
	for (; begin != end; ++begin)
	{
		out += separator + prefix + *begin + suffix;
	}
	return out;
}
template <typename ...Rest>
feUWord feStringUtil::_joinLength(feStringView first, const Rest &...rest)
{
	return first.length() + _joinLength(rest...);
}
template <typename ...Rest>
void feStringUtil::_join(feString &out, feStringView first, const Rest &...rest)
{
	out += first;
	_join(out, rest...);
}
template <typename ...Rest>
void feStringUtil::_joinWrapped(feString &out, feStringView prefix, feStringView suffix, feStringView separator, feStringView first, const Rest &...rest)
{
	out += prefix;
	out += first;
	out += suffix;
	out += separator;
	_joinWrapped(out, prefix, suffix, separator, rest...);
}