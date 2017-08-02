#include <cstdio>

#include <clang-c/Index.h>

int main(int argc, char **argv)
{
	auto index = clang_createIndex(0, 0);
	auto translationUnit = clang_createTranslationUnitFromSourceFile(
		index,
		argv[1], argc - 2, argv + 2,
		0,
		nullptr);

	clang_disposeTranslationUnit(translationUnit);
	clang_disposeIndex(index);

	printf("%s -> %s\n", argv[0], argv[1]);

	return 0;
}