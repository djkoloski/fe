#include <Fe/Core/FeEntry.h>

#include <Fe/Core/FeConfig.h>

// Include catch if unit tests are enabled
#if FE_USING(FE_FEATURE_UNIT_TESTS)

#define CATCH_CONFIG_RUNNER
#include <External/catch.hpp>

#endif

// Parse arguments and call feMain
static int parseArguments(int argc, char **argv)
{
#if FE_USING(FE_FEATURE_UNIT_TESTS)
	if (argc > 1 && !strcmp(argv[1], "--run-unit-tests"))
	{
		return Catch::Session().run(argc - 1, argv + 1);
	}
#endif

	feMain(argc, argv);
	return 0;
}

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Core/FeConfig.h>

// Subsystem Console if print is enabled
#if FE_USING(FE_FEATURE_PRINT)

int main(int argc, char **argv)
{
	parseArguments(argc, argv);
	return 0;
}

// Subsystem Windows otherwise
#else

#include <Fe/Core/FeWindows.h>
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	parseArguments(__argc, __argv);
	return 0;
}

#endif

#endif