#include <Fe/Core/FeEntry.h>

#include <Fe/Core/FeConfig.h>
#include <Fe/Core/FePrint.h>

// Include catch if unit tests are enabled
#if FE_USING(FE_FEATURE_UNIT_TESTS)

#define CATCH_CONFIG_RUNNER
#include <catch.hpp>

struct FeUnitTestListener : Catch::TestEventListenerBase
{
	using TestEventListenerBase::TestEventListenerBase;

	virtual bool assertionEnded(const Catch::AssertionStats &assertionStats) override
	{
		auto sourceLineInfo = assertionStats.assertionResult.getSourceInfo();
		switch (assertionStats.assertionResult.getResultType())
		{
		case Catch::ResultWas::Ok:
			break;
		case Catch::ResultWas::Warning:
			FE_PRINT(
				"%s(%llu): warning UnitTest: %s\n",
				sourceLineInfo.file,
				sourceLineInfo.line,
				assertionStats.assertionResult.getMessage());
			break;
		default:
			FE_PRINT(
				"%s(%llu): error UnitTest: FAILED: \"%s\" with expansion: \"%s\"\n",
				sourceLineInfo.file,
				sourceLineInfo.line,
				assertionStats.assertionResult.getExpressionInMacro().c_str(),
				assertionStats.assertionResult.getExpandedExpression().c_str());
			break;
		}

		return false;
	}
};
CATCH_REGISTER_LISTENER(FeUnitTestListener);

#endif

// Parse arguments and call feMain
static feInt runMain(feInt argc, const feRawString *argv)
{
#if FE_USING(FE_FEATURE_UNIT_TESTS)
	if (argc > 1 && !strcmp(argv[1], "--run-unit-tests"))
	{
		return Catch::Session().run(argc - 1, argv + 1);
	}
#endif

	return feMain(argc, argv);
}

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Core/FeConfig.h>

// Subsystem Console if print is enabled
#if FE_USING(FE_FEATURE_PRINT)

int main(int argc, char **argv)
{
	return runMain(argc, argv);
}

// Subsystem Windows otherwise
#else

#include <Fe/Core/FeWindows.h>
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	return runMain(__argc, __argv);
}

#endif

#endif