#include <CGen/CursorUtil.h>
#include <CGen/StringUtil.h>

#include <iostream>
#include <sstream>
#include <fstream>

class Writer
{
private:
	size_t _offset;
	std::ifstream _inputHeader;
	std::stringstream _outputHeader;
	std::stringstream _outputSource;
public:
	Writer(const std::string &inputHeaderPath, const std::string &outputHeaderPath, const std::string &outputSourcePath);

	std::stringstream &header();
	const std::stringstream &header() const;
	std::stringstream &source();
	const std::stringstream &source() const;

	void advanceToOffset(size_t offset);
	void advanceToCursorStart(CXCursor cursor, int shift = 0);
	void advanceToCursorEnd(CXCursor cursor, int shift = 0);
};

Writer::Writer(const std::string &inputHeaderPath, const std::string &outputHeaderPath, const std::string &outputSourcePath) :
	_offset(0),
	_inputHeader(inputHeaderPath, std::ios::binary)
{}
std::stringstream &Writer::header()
{
	return _outputHeader;
}
const std::stringstream &Writer::header() const
{
	return _outputHeader;
}
std::stringstream &Writer::source()
{
	return _outputSource;
}
const std::stringstream &Writer::source() const
{
	return _outputSource;
}
void Writer::advanceToOffset(size_t offset)
{
	if (offset < _offset)
	{
		return;
	}

	std::string buffer;
	buffer.resize(offset - _offset);
	_inputHeader.read(&buffer[0], buffer.size());
	_outputHeader.write(&buffer[0], buffer.size());
	_offset = offset;
}
void Writer::advanceToCursorStart(CXCursor cursor, int shift)
{
	auto extent = clang_getCursorExtent(cursor);
	auto start = clang_getRangeStart(extent);
	unsigned int offset;
	clang_getFileLocation(start, nullptr, nullptr, nullptr, &offset);
	advanceToOffset(offset + shift);
}
void Writer::advanceToCursorEnd(CXCursor cursor, int shift)
{
	auto extent = clang_getCursorExtent(cursor);
	auto start = clang_getRangeEnd(extent);
	unsigned int offset;
	clang_getFileLocation(start, nullptr, nullptr, nullptr, &offset);
	advanceToOffset(offset + shift);
}

void parseField(CXCursor cursor, CXCursor parent, std::string &metaData, Writer &writer)
{
	auto parentSpelling = CursorUtil::getSpelling(parent);
	auto type = clang_getCursorType(cursor);
	auto spelling = CursorUtil::getSpelling(cursor);
	auto name = StringUtil::fieldName(spelling);
	auto properName = StringUtil::fieldProperName(spelling);
	auto typeSpelling = CursorUtil::getSpelling(type);
	auto attribs = CursorUtil::getAttributes(cursor);

	writer.header()
		// Type declaration
		<< "public:\r\n\tusing Field"
		<< properName <<
		"Type = "
		<< typeSpelling << ";\r\n";

	metaData +=
		"\t\tfeMetaField(sizeof("
		+ parentSpelling
		+ "::"
		+ spelling
		+ "), offsetof("
		+ parentSpelling
		+ ", "
		+ spelling
		+ ")),\r\n";

	bool hasGet = attribs.count("Get");
	bool hasSet = attribs.count("Set");

	if (hasGet)
	{
		// Getter
		writer.header()
			<< "\tconst Field"
			<< properName
			<< "Type &get"
			<< properName
			<< "() const;\r\n";

		writer.source()
			<< "const "
			<< parentSpelling
			<< "::Field"
			<< properName
			<< "Type &"
			<< parentSpelling
			<< "::get"
			<< properName
			<< "() const\r\n{\r\n\treturn "
			<< spelling << ";\r\n}\r\n";
	}

	if (hasSet)
	{
		// Setter
		writer.header()
			<< "\tvoid set"
			<< properName
			<< "(const Field"
			<< properName
			<< "Type &value);\r\n";

		writer.source()
			<< "void "
			<< parentSpelling
			<< "::set"
			<< properName
			<< "(const "
			<< parentSpelling
			<< "::Field"
			<< properName
			<< "Type &value)\r\n{\r\n\t"
			<< spelling << " = value;\r\n}\r\n";
	}
}

class ParseClassData
{
public:
	Writer &writer;
	std::string &metaData;
	size_t &metaFieldCount;

	ParseClassData(Writer &writer, std::string &metaData, size_t &metaFieldCount);
};

ParseClassData::ParseClassData(Writer &writer, std::string &metaData, size_t &metaFieldCount) :
	writer(writer),
	metaData(metaData),
	metaFieldCount(metaFieldCount)
{}

void parseClass(CXCursor cursor, Writer &writer)
{
	auto spelling = CursorUtil::getSpelling(cursor);

	writer.advanceToCursorEnd(cursor, -1);
	writer.header() << "\r\n// CGen generated code\r\n";

	auto metaData = std::string();
	auto metaFieldCount = size_t(0);
	auto data = ParseClassData(writer, metaData, metaFieldCount);

	clang_visitChildren(
		cursor,
		[](CXCursor cursor, CXCursor parent, CXClientData clientData)
		{
			auto &data = *static_cast<ParseClassData *>(clientData);

			switch (clang_getCursorKind(cursor))
			{
			case CXCursor_FieldDecl:
				++data.metaFieldCount;
				parseField(cursor, parent, data.metaData, data.writer);
				break;
			default:
				break;
			}
			return CXChildVisit_Continue;
		},
		(void *)&data);

	writer.header() << "public:\r\n\tstatic const feMetaObject *getClassMeta();\r\n\tvirtual const feMetaObject *getMetaObject() override; \r\n";
	writer.source()
		<< "const feMetaObject *"
		<< spelling
		<< "::getClassMeta()\r\n{\r\n\tstatic const auto k_"
		<< spelling
		<< "MetaFieldCount = "
		<< metaFieldCount
		<< ";\r\n\tstatic const feMetaField k_"
		<< spelling
		<< "MetaFields[k_"
		<< spelling
		<< "MetaFieldCount] = {\r\n"
		<< metaData
		<< "\t};\r\n\tstatic const auto k_"
		<< spelling
		<< "MetaObject = feMetaObject(k_"
		<< spelling
		<< "MetaFieldCount, k_"
		<< spelling
		<< "MetaFields);\r\n\treturn &k_"
		<< spelling
		<< "MetaObject;\r\n}\r\nconst feMetaObject *TestClass::getMetaObject()\r\n{\r\n\treturn getClassMeta();\r\n}\r\n";
}

void parseTranslationUnit(CXCursor cursor, Writer &writer)
{
	clang_visitChildren(
		cursor,
		[](CXCursor cursor, CXCursor parent, CXClientData clientData)
		{
			auto &writer = *static_cast<Writer *>(clientData);

			// Only process cursors in our source file
			if (!clang_Location_isFromMainFile(clang_getCursorLocation(cursor)))
			{
				return CXChildVisit_Continue;
			}

			switch (clang_getCursorKind(cursor))
			{
			case CXCursor_InclusionDirective:
				break;
			case CXCursor_ClassDecl:
				parseClass(cursor, writer);
				break;
			default:
				break;
			}
			return CXChildVisit_Continue;
		},
		(void *)&writer);

	writer.advanceToCursorEnd(cursor);
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

	// Report diagnostics
	auto numDiagnostics = clang_getNumDiagnostics(translationUnit);
	for (auto i = (unsigned int)0; i < numDiagnostics; ++i)
	{
		auto diagnostic = clang_getDiagnostic(translationUnit, i);

		if (clang_Location_isInSystemHeader(clang_getDiagnosticLocation(diagnostic)))
		{
			continue;
		}

		auto string = clang_formatDiagnostic(diagnostic, CXDiagnostic_DisplaySourceLocation);

		std::cerr << clang_getCString(string) << std::endl;

		clang_disposeString(string);
		clang_disposeDiagnostic(diagnostic);
	}

	auto writer = Writer(inputHeaderPath, outputHeaderPath, outputSourcePath);

	// Parse translation unit
	writer.header()
		<< "// Generated by CGen. DO NOT EDIT!\r\n"
		<< "// Input header file: " << inputHeaderPath << "\r\n\r\n";

	writer.source()
		<< "// Generated by CGen. DO NOT EDIT!\r\n"
		<< "// Input header file: " << inputHeaderPath << "\r\n\r\n"
		<< "#include \"" << BaseName(outputHeaderPath) << "\"\r\n\r\n";

	auto cursor = clang_getTranslationUnitCursor(translationUnit);

	parseTranslationUnit(cursor, writer);

	clang_disposeTranslationUnit(translationUnit);
	clang_disposeIndex(index);

	// Write output
	auto headerOutput = std::ofstream(outputHeaderPath, std::ios::binary);
	headerOutput << writer.header().str();
	headerOutput.close();

	auto sourceOutput = std::ofstream(outputSourcePath, std::ios::binary);
	sourceOutput << writer.source().str();
	sourceOutput.close();

	return 0;
}