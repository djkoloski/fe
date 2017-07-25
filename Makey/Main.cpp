#include <Fe/Container/FeGUID.h>
#include <Fe/System/Directory.h>
#include <Makey/Solution.h>
#include <Makey/NinjaFile.h>

void ConfigureSettings(Solution &solution, feInt argc, const feRawString *argv);
void ConfigureRules(Solution &solution);
void BuildProject(Project &project, Solution &solution);
void ConfigureFe(Solution &solution);
void ConfigureMakey(Solution &solution);
void ConfigureSolutionFolders(Solution &solution);
void WriteNinjaFile(Solution &solution);
void WriteMSVCSolution(Solution &solution);

void feMain(feInt argc, const feRawString *argv)
{
	auto solutionGUID = feGUID(
		0x8B, 0xC9, 0xCE, 0xB8,
		0x8B, 0x4A, 0x11, 0xD0,
		0x8D, 0x11, 0x00, 0xA0,
		0xC9, 0x1B, 0xC9, 0x42);
	auto solution = Solution("Breakout", solutionGUID);

	solution.setSolutionDir(".");
	ConfigureSettings(solution, argc, argv);
	ConfigureRules(solution);
	ConfigureFe(solution);
	ConfigureMakey(solution);
	ConfigureSolutionFolders(solution);

	WriteNinjaFile(solution);
	WriteMSVCSolution(solution);
}

const auto g_kUsage =
	"usage: Makey [variable=value...]\n"
	"\n"
	"variables:\n"
	"  platform\tThe platform to generate ninja build files for\n"
	"    win_x86\tWindows 32-bit\n"
	"    win_x64\tWindows 64-bit\n"
	"  config\tThe configuration to generate ninja build files for\n"
	"    debug\tIncludes debugging symbols, asserts on, optimizations off\n"
	"    release\tIncludes debugging symbols, asserts on, some optimizations on\n"
	"    profile\tNo debugging symbols, asserts on, optimizations on\n"
	"    final\tNo debugging symbols, asserts off, optimizations on\n";

void ConfigureSettings(Solution &solution, feInt argc, const feRawString *argv)
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
				FE_LOG_WARNING("Duplicate compiler, was previously set to %s\n", settings.getCompilerString());
			}

			if (value == "msvc")
			{
				settings.setCompiler(Settings::Compiler::MSVC);
			}
			else
			{
				FE_LOG_WARNING("Unrecognized compiler '%s'\n", value.c_str());
			}
		}
		else if (variable == "platform")
		{
			if (settings.getPlatform() != Settings::Platform::Unknown)
			{
				FE_LOG_WARNING("Duplicate platform, was previously set to %s\n", settings.getPlatformString());
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
				FE_LOG_WARNING("Unrecognized platform '%s'\n", value.c_str());
			}
		}
		else if (variable == "config")
		{
			if (settings.getConfiguration() != Settings::Configuration::Unknown)
			{
				FE_LOG_WARNING("Duplicate configuration, was previously set to %s\n", settings.getConfigurationString());
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
				FE_LOG_WARNING("Unrecognized configuration '%s'\n", value.c_str());
			}
		}
		else
		{
			FE_LOG_WARNING("Unrecognized variable '%s' with value '%s'\n", variable.c_str(), value.c_str());
		}
	}

	if (settings.getCompiler() == Settings::Compiler::Unknown)
	{
		FE_LOG_ERROR("Compiler not set\n%s", g_kUsage);
		feExit();
	}
	if (settings.getPlatform() == Settings::Platform::Unknown)
	{
		FE_LOG_ERROR("Platform not set\n%s", g_kUsage);
		feExit();
	}
	if (settings.getConfiguration() == Settings::Configuration::Unknown)
	{
		FE_LOG_ERROR("Configuration not set\n%s", g_kUsage);
		feExit();
	}

	solution.setSettings(settings);
}

void ConfigureRules(Solution &solution)
{
	auto &compile = *solution.addRule("compile");
	compile.setDescription("compile $out");
	auto compileCommand = feString();

	auto &link = *solution.addRule("link");
	link.setDescription("link $out");
	auto linkCommand = feString();

	auto &lib = *solution.addRule("lib");
	lib.setDescription("lib $out");
	auto libCommand = feString();

	auto &copy = *solution.addRule("copy");
#if FE_IS_TARGET(WINDOWS)
	copy.setCommand("cmd /k copy $in $out");
#endif
	copy.setDescription("copy $in $out");

	switch (solution.getSettings().getCompiler())
	{
	case Settings::Compiler::MSVC:
		// Compile
		compileCommand = "cl /c $in /Fo$out /showIncludes /I $solutionDir $includes /nologo /W3 /sdl /WX /EHsc /GR- /fp:fast /vms";
		compile.setDeps("msvc");

		// Link
		linkCommand = "link $in $libPaths $libs /OUT:$out /nologo /WX";

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
			compileCommand += " /DCONFIG_RELEASE /Z7 /O2";
			linkCommand += " /SUBSYSTEM:CONSOLE /DEBUG /OPT:REF /OPT:ICF";
			libCommand += " /SUBSYSTEM:CONSOLE";
			break;
		case Settings::Configuration::Profile:
			// Profile
			compileCommand += " /DCONFIG_PROFILE /O2 /GL";
			linkCommand += " /SUBSYSTEM:CONSOLE /LTCG /OPT:REF /OPT:ICF";
			libCommand += " /SUBSYSTEM:CONSOLE /LTCG";
			break;
		case Settings::Configuration::Final:
			// Final
			compileCommand += " /DCONFIG_FINAL /O2 /GL";
			linkCommand += " /SUBSYSTEM:WINDOWS /LTCG /OPT:REF /OPT:ICF";
			libCommand += " /SUBSYSTEM:WINDOWS /LTCG";
			break;
		default:
			break;
		}
		break;
	default:
		break;
	}

	compile.setCommand(compileCommand);
	link.setCommand(linkCommand);
	lib.setCommand(libCommand);
}

void BuildProject(Project &project, Solution &solution)
{
	auto sourceDir = Path::join(".", project.getName());

	auto allObjects = feString();
	Directory::iterate(
		sourceDir,
		[compile = solution.getRule("compile"), &solution = solution, &project = project, &allObjects = allObjects, &sourceDir = sourceDir](const FileInfo &fileInfo)
		{
			const auto &relPath = Path::removePrefix(fileInfo.getPath(), sourceDir);
			if (Path::extension(relPath) == ".cpp")
			{
				auto source = Path::join("$sourceDir", relPath);
				auto object = Path::join("$buildDir", Path::addExtension(Path::removeExtension(relPath), solution.getSettings().getObjectFileExtension()));
				auto &build = project.addBuildCommand();
				build.setRule(compile);
				build.setInputs(source);
				build.setOutputs(object);
				allObjects = feStringUtil::append(allObjects, object);
			}
		},
		true);

	auto outputPath = Path::join("$binDir", project.getName());
	switch (project.getType())
	{
	case Project::Type::Executable:
		outputPath = Path::addExtension(outputPath, solution.getSettings().getExecutableFileExtension());
		break;
	case Project::Type::Library:
		outputPath = Path::addExtension(outputPath, solution.getSettings().getLibraryFileExtension());
		break;
	default:
		break;
	}

	auto &combine = project.addBuildCommand();
	switch (project.getType())
	{
	case Project::Type::Executable:
		combine.setRule(solution.getRule("link"));
		break;
	case Project::Type::Library:
		combine.setRule(solution.getRule("lib"));
		break;
	default:
		break;
	}
	combine.setInputs(allObjects);
	combine.setOutputs(outputPath);
	auto allDependencies = feString();
	for (const auto *module : project.getModules())
	{
		for (const auto *project : module->getDependencies())
		{
			allDependencies = feStringUtil::append(allDependencies, project->getName());
		}
	}
	combine.setImplicitDependencies(allDependencies);

	auto &alias = project.addBuildCommand();
	alias.setRule(null);
	alias.setInputs(outputPath);
	alias.setOutputs(project.getName());
}

void ConfigureFe(Solution &solution)
{
	auto visualStudioGUID = feGUID(
		0x0D, 0x05, 0xEF, 0x5D,
		0x0D, 0x1A, 0x4B, 0x14,
		0xAC, 0x37, 0xB2, 0x74,
		0xEC, 0x62, 0xE5, 0x88);
	auto &project = *solution.addProject("Fe", visualStudioGUID);
	project.setType(Project::Type::Library);

	BuildProject(project, solution);

	// Module
	auto &module = *solution.addModule("Fe");

	module.addInclude("$solutionDir");
	module.addLib(
		Path::join(
			"$solutionDir",
			"Bin",
			"$buildType",
			project.getName(),
			Path::addExtension(
				project.getName(),
				solution.getSettings().getLibraryFileExtension())));
	module.addDependency(solution.getProject("Fe"));
}

void ConfigureMakey(Solution &solution)
{
	auto visualStudioGUID = feGUID(
		0x53, 0x75, 0x11, 0xC5,
		0x42, 0xF5, 0x46, 0xDB,
		0xBA, 0xA2, 0x19, 0xC3,
		0x5C, 0x15, 0x49, 0x58);
	auto &project = *solution.addProject("Makey", visualStudioGUID);
	project.setType(Project::Type::Executable);
	project.addModule(solution.getModule("Fe"));

	BuildProject(project, solution);
}

void ConfigureSolutionFolders(Solution &solution)
{
	auto solutionFoldersGUID = feGUID(
		0x21, 0x50, 0xE3, 0x33,
		0x8F, 0xDC, 0x42, 0xA3,
		0x94, 0x74, 0x1A, 0x39,
		0x56, 0xD4, 0x6D, 0xE8);
	solution.setVisualStudioSolutionFoldersGUID(solutionFoldersGUID);

	auto solutionFolderGUID = feGUID(
		0xFF, 0x3D, 0xB7, 0x3F,
		0xE9, 0x1E, 0x46, 0x05,
		0xAB, 0x14, 0x4C, 0x27,
		0xC4, 0xF6, 0x95, 0xA8);
	auto *solutionFolder = solution.addVisualStudioSolutionFolder("Solution Items", solutionFolderGUID);
	Directory::iterate(
		"*",
		[solutionFolder = solutionFolder](const FileInfo &fileInfo)
		{
			if (fileInfo.getType() == FileType::File)
			{
				const auto &path = fileInfo.getPath();
				if (Path::extension(path) != ".sln")
				{
					solutionFolder->addPath(fileInfo.getPath());
				}
			}
		});
	solutionFolder->addPath(".gitignore");
	solutionFolder->addPath("build.ninja");
}

void WriteNinjaFile(Solution &solution)
{
	auto n = NinjaFile("build.ninja");

	// Rules
	n.writeComment("Rules");
	for (const auto &i : solution.getRules())
	{
		n.writeRule(*i.second);
	}

	// Variables
	n.writeNewline();
	n.writeComment("Variables");
	n.writeVariable("solutionDir", Directory::currentWorkingDirectory());
	n.writeVariable("buildType", solution.getSettings().getIdentifier());

	// Modules
	for (const auto &pair : solution.getModules())
	{
		const auto &module = *pair.second;
		n.writeNewline();
		n.writeComment(module.getName() + " Module");
		n.writeVariable(module.getName() + "Includes", feStringUtil::joinRangeWrapped("/I ", "", " ", module.getIncludes().begin(), module.getIncludes().end()));
		auto allLibs = feString();
		auto allLibPaths = feString();
		for (const auto &libPath : module.getLibs())
		{
			allLibs = feStringUtil::append(allLibs, Path::baseName(libPath));
			allLibPaths = feStringUtil::append(allLibPaths, "/LIBPATH:" + Path::dirName(libPath));
		}
		n.writeVariable(module.getName() + "Libs", allLibs);
		n.writeVariable(module.getName() + "LibPaths", allLibPaths);
	}

	// Projects
	for (const auto &pair : solution.getProjects())
	{
		const auto &project = *pair.second;
		n.writeNewline();
		n.writeComment(project.getName());
		auto ninjaPath = Path::join("$solutionDir", project.getName(), "build.ninja");
		n.writeSubninja(ninjaPath);

		// Project NinjaFile
		auto n = NinjaFile(Path::join(project.getName(), "build.ninja"));

		// Variables
		n.writeComment("Variables");
		n.writeVariable("project", project.getName());
		n.writeVariable("sourceDir", Path::join("$solutionDir", "$project"));
		n.writeVariable("buildDir", Path::join("$solutionDir", "Build", "$buildType", "$project"));
		n.writeVariable("binDir", Path::join("$solutionDir", "Bin", "$buildType", "$project"));
		n.writeNewline();

		// Modules
		n.writeComment("Modules");
		auto includes = feString();
		auto libPaths = feString();
		auto libs = feString();
		for (const auto *module : project.getModules())
		{
			includes = feStringUtil::append(includes, "$" + module->getName() + "Includes");
			libPaths = feStringUtil::append(libPaths, "$" + module->getName() + "LibPaths");
			libs = feStringUtil::append(libs, "$" + module->getName() + "Libs");
		}
		n.writeVariable("includes", includes);
		n.writeVariable("libPaths", libPaths);
		n.writeVariable("libs", libs);
		n.writeNewline();

		// Build
		n.writeComment("Build");
		for (const auto &command : project.getBuildCommands())
		{
			n.writeBuild(command);
		}
	}
}

void WriteMSVCSolution(Solution &solution)
{
	auto output = std::ofstream(solution.getName() + ".sln");

	// Header
	output << "\n";
	output << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
	output << "# Visual Studio 15\n";
	output << "VisualStudioVersion = 15.0.26403.0\n";
	output << "MinimumVisualStudioVersion = 10.0.40219.1\n";

	// Projects
	for (const auto &pair : solution.getProjects())
	{
		const auto &name = pair.first;
		const auto &project = *pair.second;

		output
			<< "Project(\"{"
			<< solution.getVisualStudioGUID().toString()
			<< "}\") = \""
			<< name
			<< "\", \""
			<< name
			<< "\\"
			<< name
			<< ".vcxproj\", \"{"
			<< project.getVisualStudioGUID().toString()
			<< "}\"\n";

		// Dependencies
		feBool hasDependencies = false;
		for (const auto *module : project.getModules())
		{
			for (const auto *dependency : module->getDependencies())
			{
				if (!hasDependencies)
				{
					output << "\tProjectSection(ProjectDependencies) = postProject\n";
					hasDependencies = true;
				}

				output
					<< "\t\t{"
					<< dependency->getVisualStudioGUID().toString()
					<< "} = {"
					<< dependency->getVisualStudioGUID().toString()
					<< "}\n";
			}
		}
		if (hasDependencies)
		{
			output << "\tEndProjectSection\n";
		}

		output << "EndProject\n";
	}

	// Solution Items
	if (solution.getVisualStudioSolutionFolders().size() > 0)
	{
		for (const auto &solutionFolder : solution.getVisualStudioSolutionFolders())
		{
			output
				<< "Project(\"{"
				<< solution.getVisualStudioSolutionFoldersGUID().toString()
				<< "}\") = \""
				<< solutionFolder->getName()
				<< "\", \""
				<< solutionFolder->getName()
				<< "\", \"{"
				<< solutionFolder->getVisualStudioGUID().toString()
				<< "}\"\n";

			// Solution Items
			output << "\tProjectSection(SolutionItems) = preProject\n";

			for (const auto &path : solutionFolder->getPaths())
			{
				output
					<< "\t\t"
					<< path
					<< " = "
					<< path
					<< "\n";
			}

			output << "\tEndProjectSection\n";

			output << "EndProject\n";
		}
	}

	// Global
	output << "Global\n";
	
	// Solution configuration platforms
	output << "\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n";
	auto settings = Settings();
	for (
		settings.setConfiguration(Settings::Configuration::First);
		settings.getConfiguration() <= Settings::Configuration::Last;
		settings.setConfiguration(Settings::Configuration((feInt)settings.getConfiguration() + 1)))
	{
		for (
			settings.setPlatform(Settings::Platform::First);
			settings.getPlatform() <= Settings::Platform::Last;
			settings.setPlatform(Settings::Platform((feInt)settings.getPlatform() + 1)))
		{
			output
				<< "\t\t"
				<< settings.getConfigurationString()
				<< "|"
				<< settings.getPlatformString()
				<< " = "
				<< settings.getConfigurationString()
				<< "|"
				<< settings.getPlatformString()
				<< "\n";
		}
	}
	output << "\tEndGlobalSection\n";

	// Project configuration platforms
	output << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
	for (const auto &pair : solution.getProjects())
	{
		const auto &project = *pair.second;

		auto settings = Settings();
		for (
			settings.setConfiguration(Settings::Configuration::First);
			settings.getConfiguration() <= Settings::Configuration::Last;
			settings.setConfiguration(Settings::Configuration((feInt)settings.getConfiguration() + 1)))
		{
			for (
				settings.setPlatform(Settings::Platform::First);
				settings.getPlatform() <= Settings::Platform::Last;
				settings.setPlatform(Settings::Platform((feInt)settings.getPlatform() + 1)))
			{
				output
					<< "\t\t{"
					<< project.getVisualStudioGUID().toString()
					<< "}."
					<< settings.getConfigurationString()
					<< "|"
					<< settings.getPlatformString()
					<< ".ActiveCfg = "
					<< settings.getConfigurationString()
					<< "|"
					<< settings.getMSVCPlatformString()
					<< "\n";
				output
					<< "\t\t{"
					<< project.getVisualStudioGUID().toString()
					<< "}."
					<< settings.getConfigurationString()
					<< "|"
					<< settings.getPlatformString()
					<< ".Build.0 = "
					<< settings.getConfigurationString()
					<< "|"
					<< settings.getMSVCPlatformString()
					<< "\n";
			}
		}
	}
	output << "\tEndGlobalSection\n";

	// Solution properties
	output << "\tGlobalSection(SolutionProperties) = preSolution\n";
	output << "\t\tHideSolutionNode = FALSE\n";
	output << "\tEndGlobalSection\n";

	output << "EndGlobal\n";
}