#pragma once

#include <string>
#include <unordered_set>

class StringUtil
{
public:
	static std::string trim(const std::string &str, const std::string &whitespace = " \t");
	static void split(const std::string &str, std::unordered_set<std::string> &pieces);
	static std::string fieldName(const std::string &str);
	static std::string fieldProperName(const std::string &str);
};