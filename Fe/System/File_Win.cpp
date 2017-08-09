#include <Fe/System/File_Win.h>

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Core/FeWindows.h>

#include <fstream>

using namespace std;

void WindowsFile::readAllBytes(feStringView path, feByte *&bytes)
{
	auto ignored = feUWord();

	readAllBytes(path, bytes, ignored);
}

void WindowsFile::readAllBytes(feStringView path, feByte *&bytes, feUWord &bytesRead)
{
	FE_ASSERT_ONLY(exists(path));

	auto file = ifstream(path, ios::binary|ios::ate);
	
	bytesRead = file.tellg();
	file.seekg(0, ios::beg);
	file.read((char *)bytes, bytesRead);
}

feString WindowsFile::readAllText(feStringView path)
{
	FE_ASSERT_ONLY(exists(path));

	auto fileSize = getSizeInBytes(path);

	auto buffer = new feByte[fileSize + 1];

	readAllBytes(path, buffer, fileSize);

	buffer[fileSize] = '\0';

	const auto allText = feString((char *)buffer);
	delete[] buffer;

	return allText;
}

void WindowsFile::writeAllText(feStringView path, feStringView text)
{
	FE_ASSERT_ONLY(exists(path));
}

void WindowsFile::writeAllBytes(feStringView path, feUByte *&bytes, feUWord size)
{
	FE_ASSERT_ONLY(exists(path));


}

feUWord WindowsFile::getSizeInBytes(feStringView path)
{
	FE_ASSERT_ONLY(exists(path));

	auto file = ifstream(path, ios::binary | ios::ate);

	return file.tellg();
}

void WindowsFile::create(feStringView path)
{
	FE_ASSERT_ONLY(exists(path));

	auto file = ofstream(path);
}

feBool WindowsFile::exists(feStringView path)
{
	auto attributes = GetFileAttributes(path.c_str());
	return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY) == 0);
}

#endif


#if FE_USING(FE_FEATURE_UNIT_TESTS)

#include <Fe/Core/FeUnitTest.h>

const auto k_testFileName = "unit-test-file-name.test";

SCENARIO("Files can be read and written to", "[File]")
{
	GIVEN("A valid path to a file")
	{
		auto *bytes = new feByte[128];
		auto bytesRead = feUWord();

		THEN("We attempt to read all of its bytes")
		{
			//REQUIRE(0 == 0);
			//REQUIRE_THROWS(WindowsFile::readAllBytes(k_testFileName, bytes, bytesRead));
		}
	}
}

#endif