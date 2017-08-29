#include <FeGen/CursorUtil.h>

#include <FeGen/Util.h>
#include <FeGen/StringUtil.h>

std::string CursorUtil::getSpelling(CXCursor cursor)
{
	auto spelling = clang_getCursorSpelling(cursor);
	auto result = std::string(clang_getCString(spelling));
	clang_disposeString(spelling);

	return result;
}
std::string CursorUtil::getSpelling(CXType type)
{
	auto spelling = clang_getTypeSpelling(type);
	auto result = std::string(clang_getCString(spelling));
	clang_disposeString(spelling);

	return result;
}
CX_CXXAccessSpecifier CursorUtil::getAccessSpecifier(CXCursor cursor)
{
	return clang_getCXXAccessSpecifier(cursor);
}
std::string CursorUtil::getAccessSpecifierSpelling(CX_CXXAccessSpecifier accessSpecifier)
{
	switch (accessSpecifier)
	{
	case CX_CXXPublic:
		return "public";
	case CX_CXXProtected:
		return "protected";
	case CX_CXXPrivate:
		return "private";
	default:
		break;
	}

	return "";
}
std::unordered_set<std::string> CursorUtil::getAttributes(CXCursor cursor)
{
	std::unordered_set<std::string> attribs;

	clang_visitChildren(
		cursor,
		[](CXCursor cursor, CXCursor parent, CXClientData clientData)
		{
			UNUSED(parent);

			auto &attribs = *static_cast<std::unordered_set<std::string> *>(clientData);

			switch (clang_getCursorKind(cursor))
			{
			case CXCursor_AnnotateAttr:
				StringUtil::split(CursorUtil::getSpelling(cursor), attribs);
				break;
			default:
				break;
			}
			return CXChildVisit_Continue;
		},
		(void *)&attribs);

	return attribs;
}