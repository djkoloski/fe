#pragma once

#include <string>
#include <unordered_set>

#include <clang-c\Index.h>

class CursorUtil
{
public:
	static std::string getSpelling(CXCursor cursor);
	static std::string getSpelling(CXType type);
	static CX_CXXAccessSpecifier getAccessSpecifier(CXCursor cursor);
	static std::string getAccessSpecifierSpelling(CX_CXXAccessSpecifier accessSpecifier);
	static std::unordered_set<std::string> getAttributes(CXCursor cursor);
};