#include <FeMake/ConfigureSolution.h>

#include <Fe/System/Path.h>

feStatus ConfigureSettings(Solution &solution, feInt argc, const feRawString *argv)
{
	auto settings = Settings();

	for (auto i = 1; i < argc; ++i)
	{
		auto arg = feString(argv[i]);
		auto position = arg.find('=');

		if (position == feString::npos)
		{
			continue;
		}

		auto variable = feStringUtil::toLower(arg.substr(0, position));
		auto value = feStringUtil::toLower(arg.substr(position + 1));

		if (variable == "compiler")
		{
			if (settings.getCompiler() != Settings::Compiler::Unknown)
			{
				FE_LOG_WARNING("Duplicate compiler, was previously set to %s", settings.getCompilerString());
			}

			if (value == "msvc")
			{
				settings.setCompiler(Settings::Compiler::MSVC);
			}
			else if (value == "llvm")
			{
				settings.setCompiler(Settings::Compiler::LLVM);
			}
			else
			{
				FE_LOG_WARNING("Unrecognized compiler '%s'", value.c_str());
			}
		}
		else if (variable == "platform")
		{
			if (settings.getPlatform() != Settings::Platform::Unknown)
			{
				FE_LOG_WARNING("Duplicate platform, was previously set to %s", settings.getPlatformString());
			}

			if (value == "win_x86")
			{
				settings.setPlatform(Settings::Platform::Win_x86);
			}
			else if (value == "win_x64")
			{
				settings.setPlatform(Settings::Platform::Win_x64);
			}
			else
			{
				FE_LOG_WARNING("Unrecognized platform '%s'", value.c_str());
			}
		}
		else if (variable == "config")
		{
			if (settings.getConfiguration() != Settings::Configuration::Unknown)
			{
				FE_LOG_WARNING("Duplicate configuration, was previously set to %s", settings.getConfigurationString());
			}

			if (value == "debug")
			{
				settings.setConfiguration(Settings::Configuration::Debug);
			}
			else if (value == "release")
			{
				settings.setConfiguration(Settings::Configuration::Release);
			}
			else if (value == "profile")
			{
				settings.setConfiguration(Settings::Configuration::Profile);
			}
			else if (value == "final")
			{
				settings.setConfiguration(Settings::Configuration::Final);
			}
			else
			{
				FE_LOG_WARNING("Unrecognized configuration '%s'", value.c_str());
			}
		}
		else
		{
			FE_LOG_WARNING("Unrecognized variable '%s' with value '%s'", variable.c_str(), value.c_str());
		}
	}

	if (settings.getCompiler() == Settings::Compiler::Unknown)
	{
		FE_LOG_ERROR("Compiler not set");
		return kFailure;
	}
	if (settings.getPlatform() == Settings::Platform::Unknown)
	{
		FE_LOG_ERROR("Platform not set");
		return kFailure;
	}
	if (settings.getConfiguration() == Settings::Configuration::Unknown)
	{
		FE_LOG_ERROR("Configuration not set");
		return kFailure;
	}

	solution.setSettings(settings);
	return kSuccess;
}

void ConfigureRules(Solution &solution)
{
	auto &compile = *solution.addRule("compile");
	compile.setDescription("compile $out");
	auto compileCommand = feString();

	auto &codegen = *solution.addRule("codegen");
	codegen.setRestat(true);
	codegen.setDescription("codegen $in $out");
	auto codegenFlags = "-Wall -x c++ -Xclang -DCODEGEN -Wno-pragma-once-outside-header -Wno-unused-private-field -ferror-limit=0";
	auto codegenCommand =
		Path::join(
			"$solutionDir",
			"Bin",
			"Win_x64_Release",
			"FeGen",
			Path::addExtension(
				"FeGen",
				solution.getSettings().getExecutableFileExtension()))
		+ " $in $out -I$solutionDir $codegenIncludes "
		+ codegenFlags;

	auto &link = *solution.addRule("link");
	link.setDescription("link $out");
	auto linkCommand = feString();

	auto &linkWholeArchive = *solution.addRule("linkWholeArchive");
	linkWholeArchive.setDescription("linkWholeArchive $out");
	auto linkWholeArchiveFlags = feString();

	auto &lib = *solution.addRule("lib");
	lib.setDescription("lib $out");
	auto libCommand = feString();

	auto &copy = *solution.addRule("copy");
#if FE_IS_TARGET(WINDOWS)
	copy.setCommand("cmd /c copy $in $out");
#endif
	copy.setRestat(true);
	copy.setDescription("copy $in $out");

	auto &unitTest = *solution.addRule("unitTest");
	unitTest.setCommand("$in --run-unit-tests -r xml -o $in.unittests.xml");
	unitTest.setDescription("unit test $in");

	// Codegen
	switch (solution.getSettings().getPlatform())
	{
	case Settings::Platform::Win_x86:
		codegenCommand += " -DPLATFORM_WIN_X86";
		break;
	case Settings::Platform::Win_x64:
		codegenCommand += " -DPLATFORM_WIN_X64";
		break;
	default:
		FE_ERROR_SWITCH_VALUE();
		break;
	}
	switch (solution.getSettings().getConfiguration())
	{
	case Settings::Configuration::Debug:
		codegenCommand += " -DCONFIG_DEBUG";
		break;
	case Settings::Configuration::Release:
		codegenCommand += " -DCONFIG_RELEASE";
		break;
	case Settings::Configuration::Profile:
		codegenCommand += " -DCONFIG_PROFILE";
		break;
	case Settings::Configuration::Final:
		codegenCommand += " -DCONFIG_FINAL";
		break;
	default:
		FE_ERROR_SWITCH_VALUE();
		break;
	}

	// Compile/link/lib
	switch (solution.getSettings().getCompiler())
	{
	case Settings::Compiler::MSVC:
		// Compile
		compileCommand = "cl /c $in /Fo$out /showIncludes /I " + Path::join("$solutionDir", "Build", "$buildType") + " $includes /nologo /W3 /sdl /WX /EHsc /GR- /fp:fast /vms";
		compile.setDeps("msvc");

		// Link
		linkCommand = "link $in $libPaths $libs /OUT:$out /nologo /WX";

		// Link whole archive
		linkWholeArchiveFlags = "/WHOLEARCHIVE:$project.lib";

		// Lib
		libCommand = "lib $in /OUT:$out /nologo /WX";

		switch (solution.getSettings().getPlatform())
		{
		case Settings::Platform::Win_x86:
			// Win x86
			compileCommand += " /DPLATFORM_WIN_X86";
			linkCommand += " /MACHINE:X86";
			libCommand += " /MACHINE:X86";
			break;
		case Settings::Platform::Win_x64:
			// Win x64
			compileCommand += " /DPLATFORM_WIN_X64";
			linkCommand += " /MACHINE:X64";
			libCommand += " /MACHINE:X64";
			break;
		default:
			FE_ERROR_SWITCH_VALUE();
			break;
		}

		switch (solution.getSettings().getConfiguration())
		{
		case Settings::Configuration::Debug:
			// Debug
			compileCommand += " /DCONFIG_DEBUG /Z7";
			linkCommand += " /SUBSYSTEM:CONSOLE /DEBUG /OPT:REF /OPT:ICF";
			libCommand += " /SUBSYSTEM:CONSOLE";
			break;
		case Settings::Configuration::Release:
			// Release
			compileCommand += " /DCONFIG_RELEASE /Z7 /Ot";
			linkCommand += " /SUBSYSTEM:CONSOLE /DEBUG /OPT:REF /OPT:ICF";
			libCommand += " /SUBSYSTEM:CONSOLE";
			break;
		case Settings::Configuration::Profile:
			// Profile
			compileCommand += " /DCONFIG_PROFILE /Ot /GL";
			linkCommand += " /SUBSYSTEM:CONSOLE /LTCG /OPT:REF /OPT:ICF";
			libCommand += " /SUBSYSTEM:CONSOLE /LTCG";
			break;
		case Settings::Configuration::Final:
			// Final
			compileCommand += " /DCONFIG_FINAL /Ot /GL";
			linkCommand += " /SUBSYSTEM:WINDOWS /LTCG /OPT:REF /OPT:ICF";
			libCommand += " /SUBSYSTEM:WINDOWS /LTCG";
			break;
		default:
			FE_ERROR_SWITCH_VALUE();
			break;
		}
		break;
	case Settings::Compiler::LLVM:
		// Compile
		compileCommand = "clang-cl /c $in /Fo$out /showIncludes /I " + Path::join("$solutionDir", "Build", "$buildType") + " $includes /nologo /W3 /sdl /WX /EHsc /GR- /fp:fast -fms-compatibility-version=19.00";
		compile.setDeps("msvc");

		// Link
#if FE_IS_TARGET(WINDOWS)
		linkCommand = "lld-link $in $libPaths $libs /OUT:$out /nologo /WX /ignore:4221";
#endif

		// Lib
		libCommand = "llvm-lib $in /OUT:$out /nologo /ignore:4221";

		switch (solution.getSettings().getPlatform())
		{
		case Settings::Platform::Win_x86:
			// Win x86
			compileCommand += " /DPLATFORM_WIN_X86";
			linkCommand += " /MACHINE:X86";
			libCommand += " /MACHINE:X86";
			break;
		case Settings::Platform::Win_x64:
			// Win x64
			compileCommand += " /DPLATFORM_WIN_X64";
			linkCommand += " /MACHINE:X64";
			libCommand += " /MACHINE:X64";
			break;
		default:
			FE_ERROR_SWITCH_VALUE();
			break;
		}

		switch (solution.getSettings().getConfiguration())
		{
		case Settings::Configuration::Debug:
			// Debug
			compileCommand += " /DCONFIG_DEBUG /Z7";
			linkCommand += " /SUBSYSTEM:CONSOLE /DEBUG /OPT:REF /OPT:ICF";
			break;
		case Settings::Configuration::Release:
			// Release
			compileCommand += " -DCONFIG_RELEASE /Z7 /Ot";
			linkCommand += " /SUBSYSTEM:CONSOLE /DEBUG /OPT:REF /OPT:ICF";
			break;
		case Settings::Configuration::Profile:
			// Profile
			compileCommand += " /DCONFIG_PROFILE /Ot /GL";
			linkCommand += " /SUBSYSTEM:CONSOLE /LTCG /OPT:REF /OPT:ICF";
			libCommand += " /LTCG";
			break;
		case Settings::Configuration::Final:
			// Final
			compileCommand += " /DCONFIG_PROFILE /Ot /GL";
			linkCommand += " /SUBSYSTEM:WINDOWS /LTCG /OPT:REF /OPT:ICF";
			libCommand += " /LTCG";
			break;
		default:
			FE_ERROR_SWITCH_VALUE();
			break;
		}
		break;
	default:
		FE_ERROR_SWITCH_VALUE();
		break;
	}

	compile.setCommand(compileCommand);
	codegen.setCommand(codegenCommand);
	link.setCommand(linkCommand);
	linkWholeArchive.setCommand(linkCommand + " " + linkWholeArchiveFlags);
	lib.setCommand(libCommand);
}