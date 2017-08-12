#include <FeGen/StringUtil.h>

#include <sstream>

std::string StringUtil::trim(const std::string &str, const std::string &whitespace)
{
	const auto strBegin = str.find_first_not_of(whitespace);
	if (strBegin == std::string::npos)
	{
		return "";
	}

	const auto strEnd = str.find_last_not_of(whitespace);
	const auto strRange = strEnd - strBegin + 1;

	return str.substr(strBegin, strRange);
}
void StringUtil::split(const std::string &str, std::unordered_set<std::string> &pieces)
{
	std::stringstream ss(str);

	std::string piece;
	while (ss.good())
	{
		getline(ss, piece, ',');
		pieces.insert(trim(piece));
	}
}
std::string StringUtil::fieldName(const std::string &str)
{
	if (str.length() > 1 && str[0] == '_')
	{
		return str.substr(1);
	}
	else
	{
		return str;
	}
}
std::string StringUtil::fieldProperName(const std::string &str)
{
	auto result = fieldName(str);

	if (result.length() > 0)
	{
		result[0] = toupper(result[0]);
	}

	return result;
}