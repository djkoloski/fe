#include <FeMake/WriteSolution.h>

#include <Fe/System/Directory.h>
#include <Fe/System/File.h>

#include <FeMake/NinjaFile.h>

#include <sstream>

static void WriteMSVCProject(const Project &project, const Solution &solution);
static void WriteMSVCProjectUserFile(const Solution &solution, const Project &project);
static void ReplaceFileIfChanged(feStringView path, feStringView newContents);
static void WriteFileIfNew(feStringView path, feStringView contents);
static feString NinjaFileName(const Solution &solution);
static feString NinjaPathToMSVCPath(feStringView path, const Settings &settings);
static void WriteGitAttributes();
static void WriteGitPreCommit();
static void WriteGitPostMerge();

void WriteNinjaFile(const Solution &solution)
{
	auto ninjaFileName = NinjaFileName(solution);

	// Solution NinjaFile
	auto solutionNinjaFile = NinjaFile(ninjaFileName);

	solutionNinjaFile.writeComment("Variables");
	solutionNinjaFile.writeVariable("solutionDir", solution.getSolutionDir());

	// Projects
	for (const auto &project : solution.getProjects())
	{
		auto ninjaPath = Path::join("$solutionDir", project->getName(), ninjaFileName);

		solutionNinjaFile.writeNewline();
		solutionNinjaFile.writeComment(project->getName());
		solutionNinjaFile.writeSubninja(ninjaPath);

		// Project NinjaFile
		auto n = NinjaFile(Path::join(project->getName(), ninjaFileName));

		// Rules
		n.writeComment("Rules");
		for (const auto &i : solution.getRules())
		{
			n.writeRule(*i.second);
		}

		// Solution Variables
		n.writeNewline();
		n.writeComment("Solution variables");
		n.writeVariable("solutionDir", solution.getSolutionDir());
		n.writeVariable("buildType", solution.getSettings().getIdentifier());
		n.writeVariable("binDir", Path::join("$solutionDir", "Bin", "$buildType"));
		n.writeVariable("extDir", Path::join("$solutionDir", "External"));

		// Project Variables
		n.writeNewline();
		n.writeComment("Project variables");
		n.writeVariable("project", project->getName());
		n.writeVariable("sourceDir", Path::join("$solutionDir", "$project"));
		n.writeVariable("buildDir", Path::join("$solutionDir", "Build", "$buildType", "$project"));

		// Modules
		n.writeNewline();
		n.writeComment("Compile settings");
		auto modules = feHashTable<feString, const Module *>();
		project->collectDependentModules(modules);

		auto includes = feString();
		auto codegenIncludes = feString();
		auto libPaths = feString();
		auto libs = feString();
		for (const auto &pair : modules)
		{
			const auto &module = *pair.second;

			includes = feStringUtil::append(
				includes,
				feStringUtil::joinRangeWrapped(
					"/I ",
					"",
					" ",
					module.getIncludes().begin(),
					module.getIncludes().end()));

			codegenIncludes = feStringUtil::append(
				codegenIncludes,
				feStringUtil::joinRangeWrapped(
					"-I",
					"",
					" ",
					module.getIncludes().begin(),
					module.getIncludes().end()));

			for (const auto &lib : module.getLibs())
			{
				libPaths = feStringUtil::append(
					libPaths,
					"/LIBPATH:" + Path::dirName(lib));
				libs = feStringUtil::append(
					libs,
					Path::baseName(lib));
			}
		}
		n.writeVariable("includes", includes);
		n.writeVariable("codegenIncludes", codegenIncludes);
		n.writeVariable("libPaths", libPaths);
		n.writeVariable("libs", libs);
		n.writeNewline();

		// Build
		n.writeComment("Build");
		for (const auto &command : project->getBuildCommands())
		{
			n.writeBuild(command);
		}
		n.writeBuild(project->getBuildAlias());
	}
}

void WriteMSVCSolution(const Solution &solution)
{
	auto solutionPath = solution.getName() + ".sln";
	auto output = std::stringstream();

	// Header
	output << "\n";
	output << "Microsoft Visual Studio Solution File, Format Version 12.00\n";
	output << "# Visual Studio 15\n";
	output << "VisualStudioVersion = 15.0.26403.0\n";
	output << "MinimumVisualStudioVersion = 10.0.40219.1\n";

	// Projects
	for (const auto &project : solution.getProjects())
	{
		output
			<< "Project(\"{"
			<< solution.getVisualStudioGUID().toString()
			<< "}\") = \""
			<< project->getName()
			<< "\", \""
			<< project->getName()
			<< "\\"
			<< project->getName()
			<< ".vcxproj\", \"{"
			<< project->getVisualStudioGUID().toString()
			<< "}\"\n";

		// Dependencies
		output << "\tProjectSection(ProjectDependencies) = postProject\n";
		for (const auto *module : project->getModules())
		{
			for (const auto *dependency : module->getDependencies())
			{
				output
					<< "\t\t{"
					<< dependency->getVisualStudioGUID().toString()
					<< "} = {"
					<< dependency->getVisualStudioGUID().toString()
					<< "}\n";
			}
		}
		// Also depend on FeGen if codegen is enabled
		if (project->getCodegenEnabled())
		{
			const auto &fegen = solution.getProject("FeGen");
			output
				<< "\t\t{"
				<< fegen->getVisualStudioGUID().toString()
				<< "} = {"
				<< fegen->getVisualStudioGUID().toString()
				<< "}\n";
		}
		output << "\tEndProjectSection\n";

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
	for (
		Settings::Configuration configuration = Settings::Configuration::First;
		configuration <= Settings::Configuration::Last;
		++configuration)
	{
		for (
			Settings::Platform platform = Settings::Platform::First;
			platform <= Settings::Platform::Last;
			++platform)
		{
			output
				<< "\t\t"
				<< Settings::getConfigurationString(configuration)
				<< "|"
				<< Settings::getPlatformString(platform)
				<< " = "
				<< Settings::getConfigurationString(configuration)
				<< "|"
				<< Settings::getPlatformString(platform)
				<< "\n";
		}
	}
	output << "\tEndGlobalSection\n";

	// Project configuration platforms
	output << "\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n";
	for (const auto &project : solution.getProjects())
	{
		for (
			Settings::Configuration configuration = Settings::Configuration::First;
			configuration <= Settings::Configuration::Last;
			++configuration)
		{
			for (
				Settings::Platform platform = Settings::Platform::First;
				platform <= Settings::Platform::Last;
				++platform)
			{
				output
					<< "\t\t{"
					<< project->getVisualStudioGUID().toString()
					<< "}."
					<< Settings::getConfigurationString(configuration)
					<< "|"
					<< Settings::getPlatformString(platform)
					<< ".ActiveCfg = "
					<< Settings::getConfigurationString(configuration)
					<< "|"
					<< Settings::getMSVCPlatformString(platform)
					<< "\n";
				output
					<< "\t\t{"
					<< project->getVisualStudioGUID().toString()
					<< "}."
					<< Settings::getConfigurationString(configuration)
					<< "|"
					<< Settings::getPlatformString(platform)
					<< ".Build.0 = "
					<< Settings::getConfigurationString(configuration)
					<< "|"
					<< Settings::getMSVCPlatformString(platform)
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

	ReplaceFileIfChanged(solutionPath, output.str());
	output.clear();

	for (const auto &project : solution.getProjects())
	{
		WriteMSVCProject(*project, solution);
	}
}

void WriteMSVCProject(const Project &project, const Solution &solution)
{
	auto ninjaFileName = NinjaFileName(solution);

	auto projectPath = Path::join(project.getName(), Path::addExtension(project.getName(), ".vcxproj"));
	auto output = std::stringstream();

	output << "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n";
	output << "<Project DefaultTargets=\"Build\" ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n";

	// Configuration
	output << "  <ItemGroup Label=\"ProjectConfigurations\">\n";
	for (
		Settings::Configuration configuration = Settings::Configuration::First;
		configuration <= Settings::Configuration::Last;
		++configuration)
	{
		for (
			Settings::Platform platform = Settings::Platform::First;
			platform <= Settings::Platform::Last;
			++platform)
		{
			output
				<< "    <ProjectConfiguration Include=\""
				<< Settings::getConfigurationString(configuration)
				<< "|"
				<< Settings::getMSVCPlatformString(platform)
				<< "\">\n";
			output
				<< "      <Configuration>"
				<< Settings::getConfigurationString(configuration)
				<< "</Configuration>\n";
			output
				<< "      <Platform>"
				<< Settings::getMSVCPlatformString(platform)
				<< "</Platform>\n";
			output
				<< "    </ProjectConfiguration>\n";
		}
	}
	output << "  </ItemGroup>\n";

	// Headers
	output << "  <ItemGroup>\n";
	for (const auto &path : project.getHeaderFilePaths())
	{
		output
			<< "    <ClInclude Include=\""
			<< path
			<< "\" />\n";
	}
	output << "  </ItemGroup>\n";

	// Sources
	output << "  <ItemGroup>\n";
	for (const auto &path : project.getSourceFilePaths())
	{
		output
			<< "    <ClCompile Include=\""
			<< path
			<< "\" />\n";
	}
	output << "  </ItemGroup>\n";

	// Other
	output << "  <ItemGroup>\n";
	for (const auto &path : project.getOtherFilePaths())
	{
		output
			<< "    <None Include=\""
			<< path
			<< "\" />\n";
	}
	output << "  </ItemGroup>\n";

	// Globals
	output << "  <PropertyGroup Label=\"Globals\">\n";
	output << "    <VCProjectVersion>15.0</VCProjectVersion>\n";
	output
		<< "    <ProjectGuid>{"
		<< project.getVisualStudioGUID().toString()
		<< "}</ProjectGuid>\n";
	output << "    <Keyword>MakeFileProj</Keyword>\n";
	output << "  </PropertyGroup>\n";

	// Import default properties
	output << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.Default.props\" />\n";

	// Configurations
	for (
		Settings::Configuration configuration = Settings::Configuration::First;
		configuration <= Settings::Configuration::Last;
		++configuration)
	{
		feBool useDebugLibraries = (configuration == Settings::Configuration::Debug);

		for (
			Settings::Platform platform = Settings::Platform::First;
			platform <= Settings::Platform::Last;
			++platform)
		{
			output
				<< "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='"
				<< Settings::getConfigurationString(configuration)
				<< "|"
				<< Settings::getMSVCPlatformString(platform)
				<< "'\" Label=\"Configuration\">\n";
			output
				<< "    <ConfigurationType>Makefile</ConfigurationType>\n";
			output
				<< "    <UseDebugLibraries>"
				<< (useDebugLibraries ? "true" : "false")
				<< "</UseDebugLibraries>\n";
			output
				<< "    <PlatformToolset>v141</PlatformToolset>\n";
			output
				<< "  </PropertyGroup>\n";
		}
	}

	// Import properties
	output << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.props\" />\n";

	// Extension settings
	output << "  <ImportGroup Label=\"ExtensionSettings\">\n";
	output << "  </ImportGroup>\n";

	// Shared
	output << "  <ImportGroup Label=\"Shared\">\n";
	output << "  </ImportGroup>\n";

	// Property sheets
	for (
		Settings::Configuration configuration = Settings::Configuration::First;
		configuration <= Settings::Configuration::Last;
		++configuration)
	{
		for (
			Settings::Platform platform = Settings::Platform::First;
			platform <= Settings::Platform::Last;
			++platform)
		{
			output
				<< "  <ImportGroup Label=\"PropertySheets\" Condition=\"'$(Configuration)|$(Platform)'=='"
				<< Settings::getConfigurationString(configuration)
				<< "|"
				<< Settings::getMSVCPlatformString(platform)
				<< "'\">\n";
			output
				<< "    <Import Project=\"$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props\" Condition=\"exists('$(UserRootDir)\\Microsoft.Cpp.$(Platform).user.props')\" Label=\"LocalAppDataPlatform\" />\n";
			output
				<< "  </ImportGroup>\n";
		}
	}

	// User macros
	output << "  <PropertyGroup Label=\"UserMacros\" />\n";

	// NMake settings
	auto nMakeOutput = project.getName();
	switch (project.getType())
	{
	case Project::Type::Executable:
		nMakeOutput += Settings::getExecutableFileExtension(Settings::Compiler::MSVC);
		break;
	case Project::Type::Library:
		nMakeOutput += Settings::getLibraryFileExtension(Settings::Compiler::MSVC);
		break;
	default:
		FE_ERROR_SWITCH_VALUE();
		break;
	}

	for (
		Settings::Configuration configuration = Settings::Configuration::First;
		configuration <= Settings::Configuration::Last;
		++configuration)
	{
		feBool useDebugLibraries;
		feString configurationMacro;
		switch (configuration)
		{
		case Settings::Configuration::Debug:
			useDebugLibraries = true;
			configurationMacro = "CONFIG_DEBUG";
			break;
		case Settings::Configuration::Release:
			useDebugLibraries = false;
			configurationMacro = "CONFIG_RELEASE";
			break;
		case Settings::Configuration::Profile:
			useDebugLibraries = false;
			configurationMacro = "CONFIG_PROFILE";
			break;
		case Settings::Configuration::Final:
			useDebugLibraries = false;
			configurationMacro = "CONFIG_FINAL";
			break;
		default:
			FE_ERROR_SWITCH_VALUE();
			break;
		}

		for (
			Settings::Platform platform = Settings::Platform::First;
			platform <= Settings::Platform::Last;
			++platform)
		{
			feString platformMacro;
			feString platformUCRTDirectoryName;
			switch (platform)
			{
			case Settings::Platform::Win_x86:
				platformMacro = "WIN32;PLATFORM_WIN_X86";
				platformUCRTDirectoryName = "x86";
				break;
			case Settings::Platform::Win_x64:
				platformMacro = "WIN64;PLATFORM_WIN_X64";
				platformUCRTDirectoryName = "x64";
				break;
			default:
				FE_ERROR_SWITCH_VALUE();
				break;
			}

			output
				<< "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='"
				<< Settings::getConfigurationString(configuration)
				<< "|"
				<< Settings::getMSVCPlatformString(platform)
				<< "'\">\n";
			output
				<< "    <NMakeOutput>"
				<< nMakeOutput
				<< "</NMakeOutput>\n";
			output
				<< "    <NMakePreprocessorDefinitions>"
				<< configurationMacro
				<< ";"
				<< platformMacro
				<< ";"
				<< (useDebugLibraries ? "_DEBUG" : "NDEBUG")
				<< ";$(NMakePreprocesssorDefinitions)</NMakePreprocessorDefinitions>\n";
			output
				<< "    <OutDir>$(SolutionDir)Bin\\Win_$(PlatformTarget)_$(Configuration)\\$(ProjectName)</OutDir>\n";
			output
				<< "    <IntDir>$(SolutionDir)Build\\Win_$(PlatformTarget)_$(Configuration)\\$(ProjectName)</IntDir>\n";

			// Build
			output
				<< "    <NMakeBuildCommandLine>cd $(SolutionDir)\n";
			output
				<< "Bin\\Win_x64_Release\\FeMake\\FeMake.exe platform=Win_$(PlatformTarget) config=$(Configuration) compiler=MSVC\n";
			output
				<< Path::join("$(SolutionDir)", "External", "ninja", "ninja.exe")
				<< " -C "
				<< Path::join("$(SolutionDir)", "Build", "Win_$(PlatformTarget)_$(Configuration)", "$(ProjectName)")
				<< " -f "
				<< Path::join("$(SolutionDir)", "$(ProjectName)", ninjaFileName)
				<< " $(ProjectName)</NMakeBuildCommandLine>\n";

			// Rebuild
			output
				<< "    <NMakeReBuildCommandLine>cd $(SolutionDir)\n";
			output
				<< Path::join("$(SolutionDir)", "External", "ninja", "ninja.exe")
				<< " -C "
				<< Path::join("$(SolutionDir)", "Build", "Win_$(PlatformTarget)_$(Configuration)", "$(ProjectName)")
				<< " -f "
				<< Path::join("$(SolutionDir)", "$(ProjectName)", ninjaFileName)
				<< " -t clean $(ProjectName)\n";
			output
				<< "Bin\\Win_x64_Release\\FeMake\\FeMake.exe platform=Win_$(PlatformTarget) config=$(Configuration) compiler=MSVC\n";
			output
				<< Path::join("$(SolutionDir)", "External", "ninja", "ninja.exe")
				<< " -C "
				<< Path::join("$(SolutionDir)", "Build", "Win_$(PlatformTarget)_$(Configuration)", "$(ProjectName)")
				<< " -f "
				<< Path::join("$(SolutionDir)", "$(ProjectName)", ninjaFileName)
				<< " $(ProjectName)</NMakeReBuildCommandLine>\n";

			// Clean
			output
				<< "    <NMakeCleanCommandLine>cd $(SolutionDir)\n";
			output
				<< Path::join("$(SolutionDir)", "External", "ninja", "ninja.exe")
				<< " -C "
				<< Path::join("$(SolutionDir)", "Build", "Win_$(PlatformTarget)_$(Configuration)", "$(ProjectName)")
				<< " -f "
				<< Path::join("$(SolutionDir)", "$(ProjectName)", ninjaFileName)
				<< " -t clean $(ProjectName)</NMakeCleanCommandLine>\n";

			// Include paths
			auto includePaths = Path::join("$(SolutionDir)", "Build", "Win_$(PlatformTarget)_$(Configuration)");
			auto modules = feHashTable<feString, const Module *>();
			project.collectDependentModules(modules);
			for (const auto &pair : modules)
			{
				const auto &module = *pair.second;
				for (const auto &path : module.getIncludes())
				{
					includePaths = feStringUtil::append(
						includePaths,
						NinjaPathToMSVCPath(path, solution.getSettings()),
						';');
				}
			}
			output
				<< "    <IncludePath>$(VC_IncludePath);$(WindowsSDK_IncludePath);$(UCRTContentRoot)include\\$(TargetUniversalCRTVersion)\\um;$(UCRTContentRoot)include\\$(TargetUniversalCRTVersion)\\shared;"
				<< includePaths
				<< "</IncludePath>\n";

			// Library paths
			output
				<< "    <LibraryPath>$(UCRTContentRoot)lib\\$(TargetUniversalCRTVersion)\\um\\"
				<< platformUCRTDirectoryName
				<< ";$(LibraryPath)</LibraryPath>\n";

			output
				<< "  </PropertyGroup>\n";
		}
	}

	// Build log paths
	for (
		Settings::Configuration configuration = Settings::Configuration::First;
		configuration <= Settings::Configuration::Last;
		++configuration)
	{
		for (
			Settings::Platform platform = Settings::Platform::First;
			platform <= Settings::Platform::Last;
			++platform)
		{
			output
				<< "  <ItemDefinitionGroup Condition=\"'$(Configuration)|$(Platform)'=='"
				<< Settings::getConfigurationString(configuration)
				<< "|"
				<< Settings::getMSVCPlatformString(platform)
				<< "'\">\n";
			output
				<< "    <BuildLog>\n";
			output
				<< "      <Path>$(SolutionDir)Build\\Win_$(PlatformTarget)_$(Configuration)\\$(ProjectName)\\$(ProjectName).log</Path>\n";
			output
				<< "    </BuildLog>\n";
			output
				<< "  </ItemDefinitionGroup>\n";
		}
	}

	// Import targets
	output << "  <Import Project=\"$(VCTargetsPath)\\Microsoft.Cpp.targets\" />\n";
	output << "  <ImportGroup Label=\"ExtensionTargets\">\n";
	output << "  </ImportGroup>\n";

	output << "</Project>";

	ReplaceFileIfChanged(projectPath, output.str());
	output.clear();

	WriteMSVCProjectUserFile(solution, project);
}

void WriteMSVCProjectUserFile(const Solution &solution, const Project &project)
{
	auto projectPath = Path::join(project.getName(), Path::addExtension(project.getName(), ".vcxproj.user"));
	auto output = std::stringstream();

	output
		<< "<?xml version=\"1.0\" encoding=\"utf-8\"?>\n"
		<< "<Project ToolsVersion=\"15.0\" xmlns=\"http://schemas.microsoft.com/developer/msbuild/2003\">\n"
		<< "  <PropertyGroup>\n"
		<< "    <ShowAllFiles>true</ShowAllFiles>\n"
		<< "  </PropertyGroup>\n";

	for (
		Settings::Configuration configuration = Settings::Configuration::First;
		configuration <= Settings::Configuration::Last;
		++configuration)
	{
		for (
			Settings::Platform platform = Settings::Platform::First;
			platform <= Settings::Platform::Last;
			++platform)
		{
			output
				<< "  <PropertyGroup Condition=\"'$(Configuration)|$(Platform)'=='"
				<< solution.getSettings().getConfigurationString(configuration)
				<< "|"
				<< solution.getSettings().getMSVCPlatformString(platform)
				<< "'\">\n"
				<< "    <LocalDebuggerCommand>$(SolutionDir)Bin\\Win_$(PlatformTarget)_$(Configuration)\\$(ProjectName)\\$(ProjectName)"
				<< solution.getSettings().getExecutableFileExtension()
				<< "</LocalDebuggerCommand>\n";
			output
				<< "    <LocalDebuggerCommandArguments>"
				<< project.getVisualStudioDebuggerDefaultArguments()
				<< "</LocalDebuggerCommandArguments>\n";
			output
				<< "    <LocalDebuggerWorkingDirectory>$(SolutionDir)</LocalDebuggerWorkingDirectory>\n";
			output
				<< "    <DebuggerFlavor>WindowsLocalDebugger</DebuggerFlavor>\n";
			output
				<< "  </PropertyGroup>\n";
		}
	}

	output << "</Project>";

	WriteFileIfNew(projectPath, output.str());
}

void ReplaceFileIfChanged(feStringView path, feStringView newContents)
{
	auto input = std::ifstream(path);
	auto inputStr = feString();

	if (!input.fail())
	{
		input.seekg(0, std::ios::end);
		inputStr.reserve(feUInt(input.tellg()));
		input.seekg(0, std::ios::beg);
		inputStr.assign((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
		input.close();
	}

	if (input.fail() || inputStr != newContents)
	{
		auto output = std::ofstream(path);
		output << newContents;
	}
}

void WriteFileIfNew(feStringView path, feStringView contents)
{
	if (!File::exists(path))
	{
		auto output = std::ofstream(path);
		output << contents;
	}
}

feString NinjaFileName(const Solution &solution)
{
	return Path::addExtension(solution.getName(), ".ninja");
}

feString NinjaPathToMSVCPath(feStringView path, const Settings &settings)
{
	auto result = feString(path);
	result = feStringUtil::replace(
		result,
		"$solutionDir",
		"$(SolutionDir)");
	result = feStringUtil::replace(
		result,
		"$extDir",
		Path::join(
			"$(SolutionDir)",
			"External"));
	result = feStringUtil::replace(
		result,
		"$binDir",
		Path::join(
			"$(SolutionDir)",
			settings.getIdentifier()));
	return result;
}

void WriteGitIntegration()
{
	WriteGitAttributes();
	WriteGitPreCommit();
	WriteGitPostMerge();
}

void WriteGitAttributes()
{
	auto content = std::stringstream();

	content
		<< "Bootstrap.ninja\tmerge=ours\n";

	auto output = std::ofstream(Path::join(".git", "info", "attributes"));
	output << content.str();
}

void WriteGitPreCommit()
{
	auto content = std::stringstream();

	content
		<< "#!/bin/sh\n"
		<< "./Bin/Win_x64_Release/FeMake/FeMake.exe --bootstrap\n"
		<< "git add \"./*Bootstrap.ninja\"\n";

	auto output = std::ofstream(Path::join(".git", "hooks", "pre-commit"));
	output << content.str();
}

void WriteGitPostMerge()
{
	auto content = std::stringstream();

	content
		<< "#!/bin/sh\n"
		<< "./Bin/Win_x64_Release/FeMake/FeMake.exe --bootstrap\n"
		<< "git add \"./*Bootstrap.ninja\"\n"
		<< "git commit -m \"Update ninja bootstraps\"\n";

	auto output = std::ofstream(Path::join(".git", "hooks", "post-merge"));
	output << content.str();
}