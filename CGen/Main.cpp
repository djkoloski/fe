#include <Fe/Core/Core.h>

#include <clang-c/Index.h>

feInt feMain(feInt argc, const feRawString *argv)
{
	auto index = clang_createIndex(0, 0);
	auto translationUnit = clang_createTranslationUnitFromSourceFile(
		index,
		argv[1], argc - 2, argv + 2,
		0,
		null);

	clang_disposeTranslationUnit(translationUnit);
	clang_disposeIndex(index);

	FE_PRINT("Code generation goes here\n");

	return 0;
}