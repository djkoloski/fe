#include <Fe/System/File.h>

#include <iostream>
#include <fstream>

feStatus File::readAllBytes(feStringView path, feChar *&chars, feUWord &charsRead)
{
	using namespace std;

	ifstream file(path, ios::in|ios::binary|ios::ate);

	if (!file.is_open())
	{
		return kFailure;
	}

	charsRead = file.tellg();
	file.seekg(0, ios::beg);
	file.read(chars, charsRead);
	file.close();

	return kSuccess;
}

#if FE_USING(FE_FEATURE_UNIT_TESTS)

#include <Fe/Core/FeUnitTest.h>

SCENARIO("Files can be read and written to", "[File]")
{
	GIVEN("An invalid file path")
	{
		/*auto invalidPath = "/1337/paths/dont/exist(*#$";
		
		auto *chars = new feChar[1000];
		feUWord charsRead;*/

		//auto result = File::readAllBytes(invalidPath, chars, charsRead);
		REQUIRE(false);
	}
}

TEST_CASE("Testing Catch", "[test]")
{
	REQUIRE((1 + 1) == 3);
}

#endif