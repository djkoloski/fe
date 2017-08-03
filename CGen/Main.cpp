#include <CGen/CursorUtil.h>
#include <CGen/StringUtil.h>

#include <iostream>
#include <sstream>
#include <fstream>

class Output
{
public:
	std::stringstream header;
	std::stringstream source;
};

void parseField(CXCursor cursor, CXCursor parent, Output &output, int fieldCount, bool isStatic)
{
	auto parentSpelling = CursorUtil::getSpelling(parent);
	auto accessSpecifier = clang_getCXXAccessSpecifier(cursor);
	auto type = clang_getCursorType(cursor);
	auto spelling = CursorUtil::getSpelling(cursor);
	auto name = StringUtil::fieldName(spelling);
	auto properName = StringUtil::fieldProperName(spelling);
	auto typeSpelling = CursorUtil::getSpelling(type);
	auto attribs = CursorUtil::getAttributes(cursor);

	// Type declaration
	output.header << "public:\n\tusing Field" << fieldCount << "Type = " << typeSpelling << ";\n";
	// Access specifier
	output.header << CursorUtil::getAccessSpecifierSpelling(accessSpecifier) << ":\n";
	// Field declaration
	output.header
		<< "\t"
		<< (isStatic ? "static " : "")
		<< (clang_CXXField_isMutable(cursor) ? "mutable " : "")
		<< (clang_CXXMethod_isVirtual(cursor) ? "virtual " : "")
		<< "Field" << fieldCount << "Type " << spelling;
	if (clang_CXXMethod_isDefaulted(cursor))
	{
		output.header << " = default";
	}
	if (clang_CXXMethod_isPureVirtual(cursor))
	{
		output.header << " = 0";
	}
	output.header << ";\n";

	bool hasGet = attribs.count("Get");
	bool hasSet = attribs.count("Set");

	if (hasGet || hasSet)
	{
		output.header << "public:\n";
	}

	if (hasGet)
	{
		// Getter
		output.header << "\tconst Field" << fieldCount << "Type &get" << properName << "() const;\n";

		output.source << "const " << parentSpelling << "::Field" << fieldCount << "Type &" << parentSpelling << "::get" << properName << "() const\n";
		output.source << "{\n";
		output.source << "\treturn " << spelling << ";\n";
		output.source << "}\n";
	}

	if (hasSet)
	{
		// Setter
		output.header << "\tvoid set" << properName << "(const Field" << fieldCount << "Type &value);\n";

		output.source << "void " << parentSpelling << "::set" << properName << "(const " << parentSpelling << "::Field" << fieldCount << "Type &value)\n";
		output.source << "{\n";
		output.source << "\t" << spelling << " = value;\n";
		output.source << "}\n";
	}
}

void parseConstructor(CXCursor cursor, CXCursor parent, Output &output)
{
	auto accessSpecifier = clang_getCXXAccessSpecifier(cursor);
	auto type = clang_getCursorType(cursor);
	auto spelling = CursorUtil::getSpelling(cursor);

	output.header << CursorUtil::getAccessSpecifierSpelling(accessSpecifier) << ":\n";
	output.header << "\t" << spelling << "(";
	auto argCount = clang_getNumArgTypes(type);
	for (auto i = 0; i < argCount; ++i)
	{
		auto argType = clang_getArgType(type, i);
		if (i > 0)
		{
			output.header << ", ";
		}
		output.header << CursorUtil::getSpelling(argType);
	}
	output.header << ")";
	if (clang_CXXMethod_isDefaulted(cursor))
	{
		output.header << " = default";
	}
	output.header << ";\n";
}

void parseMethod(CXCursor cursor, CXCursor parent, Output &output, int fieldCount, bool isStatic)
{
	parseField(cursor, parent, output, fieldCount, isStatic);
}

void startClassBody(Output &output)
{
	output.header << "\n{\n";
}

struct ClassParsingData
{
	Output &output;
	bool startedBody;
	bool startedBases;
	int fieldCount;

	ClassParsingData(Output &output) :
		output(output),
		startedBody(false),
		startedBases(false),
		fieldCount(0)
	{}
};

void parseClass(CXCursor cursor, Output &output)
{
	output.header << "class " << CursorUtil::getSpelling(cursor);

	auto data = ClassParsingData(output);

	clang_visitChildren(
		cursor,
		[](CXCursor cursor, CXCursor parent, CXClientData clientData)
		{
			auto &data = *static_cast<ClassParsingData *>(clientData);
			auto &output = data.output;
			auto &startedBody = data.startedBody;
			auto &startedBases = data.startedBases;
			auto &fieldCount = data.fieldCount;

			switch (clang_getCursorKind(cursor))
			{
			case CXCursor_CXXBaseSpecifier:
				output.header
					<< (startedBases ? ", " : " : ")
					<< CursorUtil::getAccessSpecifierSpelling(
						CursorUtil::getAccessSpecifier(cursor))
					<< " "
					<< CursorUtil::getSpelling(cursor).substr(6);
				startedBases = true;
				break;
			case CXCursor_FieldDecl:
				if (!startedBody)
				{
					startClassBody(output);
					startedBody = true;
				}
				parseField(cursor, parent, output, fieldCount++, false);
				break;
			case CXCursor_VarDecl:
				if (!startedBody)
				{
					startClassBody(output);
					startedBody = true;
				}
				parseField(cursor, parent, output, fieldCount++, true);
				break;
			case CXCursor_CXXMethod:
				if (!startedBody)
				{
					startClassBody(output);
					startedBody = true;
				}
				parseMethod(cursor, parent, output, fieldCount++, clang_Cursor_getStorageClass(cursor) == CX_SC_Static);
				break;
			case CXCursor_Constructor:
				if (!startedBody)
				{
					startClassBody(output);
					startedBody = true;
				}
				parseConstructor(cursor, parent, output);
				break;
			default:
				break;
			}
			return CXChildVisit_Continue;
		},
		(void *)&data);

	if (!data.startedBody)
	{
		startClassBody(output);
		data.startedBody = true;
	}
	output.header << "};\n\n";
}

void parseTranslationUnit(CXCursor cursor, Output &output)
{
	clang_visitChildren(
		cursor,
		[](CXCursor cursor, CXCursor parent, CXClientData clientData)
		{
			auto &output = *static_cast<Output *>(clientData);

			// Only process cursors in our source file
			if (!clang_Location_isFromMainFile(clang_getCursorLocation(cursor)))
			{
				return CXChildVisit_Continue;
			}

			switch (clang_getCursorKind(cursor))
			{
			case CXCursor_InclusionDirective:
				output.header << "#include <" << CursorUtil::getSpelling(cursor) << ">\n\n";
				break;
			case CXCursor_ClassDecl:
				parseClass(cursor, output);
				break;
			default:
				break;
			}
			return CXChildVisit_Continue;
		},
		(void *)&output);
}

static size_t BaseNameLength(const std::string &path)
{
	for (auto i = path.length() - 1; ; --i)
	{
#if defined(PLATFORM_WIN_X86) || defined(PLATFORM_WIN_X64)
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

std::string BaseName(const std::string &path)
{
	auto i = BaseNameLength(path);
	return path.substr(path.length() - i, i);
}

int UnitTest()
{
	return 0;
}

static const auto k_usageString =
	"usage: CGen input output [flags]\n"
	"variables:\n"
	"  flags\tThe flags to pass to clang while compiling";

int main(int argc, char **argv)
{
	if (argc < 3)
	{
		std::cout << k_usageString << std::endl;
		return 1;
	}

	if (!strcmp(argv[1], "--run-unit-tests"))
	{
		return UnitTest();
	}

	auto inputHeaderPath = std::string(argv[1]);
	auto outputHeaderPath = std::string(argv[2]);
	auto outputSourcePath = outputHeaderPath;
	outputSourcePath.replace(outputSourcePath.end() - 2, outputSourcePath.end(), "-cgen.cpp");
	std::cout << outputSourcePath << std::endl;

	// ---------------------------------------------------------------------
	// TODO: remove after thorough testing
	if (inputHeaderPath != "D:\\Projects\\Fe\\Test\\Test.h")
	{
		system(("copy " + inputHeaderPath + " " + outputHeaderPath).c_str());
		system(("type nul > " + outputSourcePath).c_str());
		return 0;
	}
	// ---------------------------------------------------------------------

	auto index = clang_createIndex(0, 0);
	auto translationUnit = clang_createTranslationUnitFromSourceFile(
		index,
		argv[1], argc - 3, argv + 3,
		0,
		nullptr);

	auto numDiagnostics = clang_getNumDiagnostics(translationUnit);
	for (auto i = (unsigned int)0; i < numDiagnostics; ++i)
	{
		auto diagnostic = clang_getDiagnostic(translationUnit, i);
		auto string = clang_formatDiagnostic(diagnostic, CXDiagnostic_DisplaySourceLocation);

		std::cerr << clang_getCString(string) << std::endl;

		clang_disposeString(string);
		clang_disposeDiagnostic(diagnostic);
	}

	auto cursor = clang_getTranslationUnitCursor(translationUnit);
	auto output = Output();

	output.header << "// Generated by CGen. DO NOT EDIT!\n";
	output.header << "// Input header file: " << inputHeaderPath << "\n\n";
	output.header << "#pragma once\n\n";

	output.source << "// Generated by CGen. DO NOT EDIT!\n";
	output.source << "// Input header file: " << inputHeaderPath << "\n\n";
	output.source << "#include \"" << BaseName(outputHeaderPath) << "\"\n\n";

	parseTranslationUnit(cursor, output);

	clang_disposeTranslationUnit(translationUnit);
	clang_disposeIndex(index);

	auto headerOutput = std::ofstream(outputHeaderPath);
	headerOutput << output.header.str();
	headerOutput.close();

	auto sourceOutput = std::ofstream(outputSourcePath);
	sourceOutput << output.source.str();
	sourceOutput.close();

	return 0;
}