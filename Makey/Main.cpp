#include <Fe/System/Directory.h>
#include <Makey/Solution.h>
#include <Makey/NinjaFile.h>

const feRawString g_kUsage =
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
	Settings settings;
	
	for (feInt i = 1; i < argc; ++i)
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
	feString compileCommand;
	
	auto &link = *solution.addRule("link");
	link.setDescription("link $out");
	feString linkCommand;
	
	auto &lib = *solution.addRule("lib");
	lib.setDescription("lib $out");
	feString libCommand;
	
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
	
	feString allObjects;
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
	feString allDependencies;
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
	auto &project = *solution.addProject("Fe");
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
	auto &project = *solution.addProject("Makey");
	project.setType(Project::Type::Executable);
	project.addModule(solution.getModule("Fe"));
	
	BuildProject(project, solution);
}

void feMain(feInt argc, const feRawString *argv)
{
	Solution solution;
	
	solution.setSolutionDir(".");
	ConfigureSettings(solution, argc, argv);
	ConfigureRules(solution);
	ConfigureFe(solution);
	ConfigureMakey(solution);
	
	NinjaFile n ("build.ninja");
	
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
		feString allLibs;
		feString allLibPaths;
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
		NinjaFile n (Path::join(project.getName(), "build.ninja"));
		
		// Variables
		n.writeComment("Variables");
		n.writeVariable("project", project.getName());
		n.writeVariable("sourceDir", Path::join("$solutionDir", "$project"));
		n.writeVariable("buildDir", Path::join("$solutionDir", "Build", "$buildType", "$project"));
		n.writeVariable("binDir", Path::join("$solutionDir", "Bin", "$buildType", "$project"));
		n.writeNewline();
		
		// Modules
		n.writeComment("Modules");
		feString includes;
		feString libPaths;
		feString libs;
		for (const auto *module: project.getModules())
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