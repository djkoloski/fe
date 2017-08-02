#include <string>
#include <iostream>
#include <sstream>
#include <fstream>

#include <clang-c/Index.h>

std::string ReadEntireFile(const std::string &path)
{
	auto input = std::ifstream(path);
	auto inputStr = std::string();

	if (!input.fail())
	{
		input.seekg(0, std::ios::end);
		inputStr.reserve(input.tellg());
		input.seekg(0, std::ios::beg);
		inputStr.assign((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
		input.close();
	}

	return inputStr;
}

void ReplaceFileIfChanged(const std::string &path, const std::string &newContents)
{
	auto inputStr = ReadEntireFile(path);

	if (inputStr != newContents)
	{
		auto output = std::ofstream(path);
		output << newContents;
	}
}

int main(int argc, char **argv)
{
	/*auto index = clang_createIndex(0, 0);
	auto translationUnit = clang_createTranslationUnitFromSourceFile(
		index,
		argv[1], argc - 2, argv + 2,
		0,
		nullptr);

	clang_disposeTranslationUnit(translationUnit);
	clang_disposeIndex(index);*/

	ReplaceFileIfChanged(argv[2], ReadEntireFile(argv[1]));

	return 0;
}