#include <Fe/System/Directory.h>

#include <FeMake/Solution.h>
#include <FeMake/ConfigureSolution.h>
#include <FeMake/WriteSolution.h>

feStatus MakeAllProjects(feInt argc, const feRawString *argv);
feStatus MakeBootstrap();
void ConfigureExternal(Solution &solution);
void ConfigureFeGen(Solution &solution);
void ConfigureFe(Solution &solution);
void ConfigureFeMake(Solution &solution);
void ConfigureFeRender(Solution &solution);
void ConfigureFeUI(Solution &solution);
void ConfigureTest(Solution &solution);
void ConfigureSolutionFolders(Solution &solution);
void PerformGitIntegration();

static const auto k_usageString =
	"usage: FeMake [variable=value...]\n"
	"variables:\n"
	"  platform\tThe platform to generate ninja build files for\n"
	"    win_x86\tWindows 32-bit\n"
	"    win_x64\tWindows 64-bit\n"
	"  config\tThe configuration to generate ninja build files for\n"
	"    debug\tIncludes debugging symbols, asserts on, optimizations off\n"
	"    release\tIncludes debugging symbols, asserts on, some optimizations on\n"
	"    profile\tNo debugging symbols, asserts on, optimizations on\n"
	"    final\tNo debugging symbols, asserts off, optimizations on\n"
	"  compiler\tThe compiler to generate ninja build files for\n"
	"    msvc\tMicrosoft Visual Studio Compiler";

feInt feMain(feInt argc, const feRawString *argv)
{
	auto bootstrap = false;
	for (auto i = 0; i < argc; ++i)
	{
		if (feStringUtil::isEqual(argv[i], "--bootstrap"))
		{
			bootstrap = true;
		}
	}

	PerformGitIntegration();

	if (!bootstrap)
	{
		if (MakeAllProjects(argc, argv) == kFailure)
		{
			FE_PRINT("\n%s\n", k_usageString);
			return 1;
		}
	}
	if (MakeBootstrap() == kFailure)
	{
		FE_PRINT("\n%s\n", k_usageString);
		return 2;
	}
	
	return 0;
}

feStatus MakeAllProjects(feInt argc, const feRawString *argv)
{
	auto solutionGUID = feGUID(
		0x8B, 0xC9, 0xCE, 0xB8,
		0x8B, 0x4A, 0x11, 0xD0,
		0x8D, 0x11, 0x00, 0xA0,
		0xC9, 0x1B, 0xC9, 0x42);
	auto solution = Solution("AllProjects", solutionGUID);

	solution.setSolutionDir(Directory::currentWorkingDirectory());
	if (ConfigureSettings(solution, argc, argv) == kFailure)
	{
		return kFailure;
	}

	ConfigureRules(solution);
	ConfigureExternal(solution);
	ConfigureFeGen(solution);
	ConfigureFe(solution);
	ConfigureFeMake(solution);
	ConfigureFeRender(solution);
	ConfigureFeUI(solution);
	ConfigureTest(solution);
	ConfigureSolutionFolders(solution);

	WriteNinjaFile(solution);
	WriteBuildNinjaFile(solution);
	WriteMSVCSolution(solution);

	return kSuccess;
}

feStatus MakeBootstrap()
{
	auto bootstrap = Solution("Bootstrap", feGUID());

	// Bootstrap uses '../..' as its solution directory because it's
	// checked into git. This forces us to run it from the command
	// line with the current working directory equal to Build\Win_x64_Release.
	bootstrap.setSolutionDir(Path::join("..", ".."));

	Settings settings;
	settings.setCompiler(Settings::Compiler::MSVC);
	settings.setConfiguration(Settings::Configuration::Release);
	settings.setPlatform(Settings::Platform::Win_x64);
	bootstrap.setSettings(settings);

	ConfigureRules(bootstrap);
	ConfigureExternal(bootstrap);
	ConfigureFeGen(bootstrap);
	ConfigureFe(bootstrap);
	ConfigureFeMake(bootstrap);

	WriteNinjaFile(bootstrap);

	return kSuccess;
}

void ConfigureExternal(Solution &solution)
{
	auto platformShortName = "";
	switch (solution.getSettings().getPlatform())
	{
	case Settings::Platform::Win_x86:
		platformShortName = "x86";
		break;
	case Settings::Platform::Win_x64:
		platformShortName = "x64";
		break;
	default:
		FE_ERROR_SWITCH_VALUE();
	}

	// catch
	auto &catchModule = *solution.addModule("catch");
	catchModule.addInclude(
		Path::join(
			"$extDir",
			"catch"));

	// libclang
	auto &libclangModule = *solution.addModule("libclang");

	auto llvmDir = Path::join("$extDir", "LLVM");
	libclangModule.addInclude(
		Path::join(
			llvmDir,
			"include"));

	libclangModule.addLib(
		Path::join(
			llvmDir,
			"lib",
			platformShortName,
			Path::addExtension(
				"libclang",
				solution.getSettings().getLibraryFileExtension())));
	libclangModule.addSharedLib(
		Path::join(
			llvmDir,
			"bin",
			platformShortName,
			Path::addExtension(
				"libclang",
				solution.getSettings().getSharedLibraryFileExtension())));

	// SDL2
	auto &sdl2Module = *solution.addModule("sdl2");

	auto sdl2Dir = Path::join("$extDir", "SDL2");
	sdl2Module.addInclude(
		Path::join(
			sdl2Dir,
			"include"));

	sdl2Module.addLib(
		Path::join(
			sdl2Dir,
			"lib",
			platformShortName,
			Path::addExtension(
				"SDL2",
				solution.getSettings().getLibraryFileExtension())));
	sdl2Module.addSharedLib(
		Path::join(
			sdl2Dir,
			"lib",
			platformShortName,
			Path::addExtension(
				"SDL2",
				solution.getSettings().getSharedLibraryFileExtension())));

	// opengl
	auto &openglModule = *solution.addModule("opengl");

	openglModule.addLib("opengl32.lib");

	// gl3w
	auto &gl3wModule = *solution.addModule("gl3w");

	gl3wModule.addInclude(
		Path::join(
			"$extDir",
			"gl3w",
			"include"));
}

void ConfigureFeGen(Solution &solution)
{
	auto visualStudioGUID = feGUID(
		0x2d, 0x71, 0x9d, 0xbb,
		0x3e, 0xd3, 0x40, 0xd7,
		0x9a, 0x68, 0xb6, 0x64,
		0x41, 0xd6, 0xb6, 0x59);
	auto &project = *solution.addProject("FeGen", visualStudioGUID);
	project.setType(Project::Type::Executable);
	project.addModule(solution.getModule("libclang"));
	project.setCodegenEnabled(false);

	project.collectFiles();
	project.makeBuildRules(solution);
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
	project.addModule(solution.getModule("catch"));
	project.setVisualStudioDebuggerDefaultArguments("--run-unit-tests");

	project.collectFiles();
	project.makeBuildRules(solution);

	// Module
	auto &module = *solution.addModule("fe");

	module.addLib(
		Path::join(
			"$binDir",
			project.getName(),
			Path::addExtension(
				project.getName(),
				solution.getSettings().getLibraryFileExtension())));
	module.addDependency(&project);
}

void ConfigureFeMake(Solution &solution)
{
	auto visualStudioGUID = feGUID(
		0x53, 0x75, 0x11, 0xC5,
		0x42, 0xF5, 0x46, 0xDB,
		0xBA, 0xA2, 0x19, 0xC3,
		0x5C, 0x15, 0x49, 0x58);
	auto &project = *solution.addProject("FeMake", visualStudioGUID);
	project.setType(Project::Type::Executable);
	project.addModule(solution.getModule("fe"));
	project.setVisualStudioDebuggerDefaultArguments("compiler=MSVC platform=Win_x64 config=Release");

	project.collectFiles();
	project.makeBuildRules(solution);
}

void ConfigureFeRender(Solution &solution)
{
	auto visualStudioGUID = feGUID(
		0x1a, 0x59, 0x64, 0xa1,
		0xf7, 0xaf, 0x48, 0x29,
		0xb3, 0x26, 0x23, 0x00,
		0x37, 0x73, 0x53, 0xe8);
	auto &project = *solution.addProject("FeRender", visualStudioGUID);
	project.setType(Project::Type::Library);
	project.addModule(solution.getModule("fe"));
	project.addModule(solution.getModule("sdl2"));
	project.addModule(solution.getModule("opengl"));
	project.addModule(solution.getModule("gl3w"));
	project.setVisualStudioDebuggerDefaultArguments("--run-unit-tests");

	project.collectFiles();
	project.makeBuildRules(solution);

	// Module
	auto &module = *solution.addModule("feRender");

	module.addLib(
		Path::join(
			"$binDir",
			project.getName(),
			Path::addExtension(
				project.getName(),
				solution.getSettings().getLibraryFileExtension())));
	module.addDependency(&project);
}

void ConfigureFeUI(Solution &solution)
{
	auto visualStudioGUID = feGUID(
		0x71, 0x39, 0x0f, 0xfd,
		0x5c, 0x6f, 0x46, 0xfd,
		0xaf, 0x2a, 0x0e, 0x95,
		0x57, 0x19, 0x69, 0x0f);
	auto &project = *solution.addProject("FeUI", visualStudioGUID);
	project.setType(Project::Type::Library);
	project.addModule(solution.getModule("feRender"));
	project.setVisualStudioDebuggerDefaultArguments("--run-unit-tests");

	project.collectFiles();
	project.makeBuildRules(solution);

	// Module
	auto &module = *solution.addModule("feUI");

	module.addLib(
		Path::join(
			"$binDir",
			project.getName(),
			Path::addExtension(
				project.getName(),
				solution.getSettings().getLibraryFileExtension())));
	module.addDependency(&project);
}

void ConfigureTest(Solution &solution)
{
	auto visualStudioGUID = feGUID(
		0x11, 0x1a, 0x98, 0x10,
		0xe1, 0xb2, 0x42, 0x0c,
		0x90, 0x87, 0x8c, 0x24,
		0x3c, 0x83, 0x21, 0x38);
	auto &project = *solution.addProject("Test", visualStudioGUID);
	project.setType(Project::Type::Executable);
	project.addModule(solution.getModule("feUI"));

	project.collectFiles();
	project.makeBuildRules(solution);
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
		".",
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
}

void PerformGitIntegration()
{
	system("git config merge.ours.name ours");
	system("git config merge.ours.driver true");

	WriteGitIntegration();
}