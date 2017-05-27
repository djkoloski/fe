#include <Fe/Core/FeEntry.h>

#if FE_IS_TARGET(WINDOWS)

#include <Fe/Core/FeConfig.h>

// Subsystem Console if print is enabled
#if FE_USING(FE_FEATURE_PRINT)

int main(int argc, char **argv)
{
	feMain(argc, argv);
	return 0;
}

// Subsystem Windows otherwise
#else

#include <Fe/Core/FeWindows.h>
int __stdcall WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
	feMain(__argc, __argv);
	return 0;
}

#endif

#endif