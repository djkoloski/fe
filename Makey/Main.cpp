#include <Fe/System/Directory.h>

#include <Makey/Solution.h>
#include <Makey/AccumulateProject.h>
#include <Makey/ConfigureSolution.h>
#include <Makey/WriteSolution.h>

void ConfigureFe(Solution &solution);
void ConfigureMakey(Solution &solution);
void ConfigureSolutionFolders(Solution &solution);

void feMain(feInt argc, const feRawString *argv)
{
	auto solutionGUID = feGUID(
		0x8B, 0xC9, 0xCE, 0xB8,
		0x8B, 0x4A, 0x11, 0xD0,
		0x8D, 0x11, 0x00, 0xA0,
		0xC9, 0x1B, 0xC9, 0x42);
	auto solution = Solution("AllProjects", solutionGUID);

	solution.setSolutionDir(".");
	if (ConfigureSettings(solution, argc, argv) == kFailure)
	{
		return;
	}

	ConfigureRules(solution);
	ConfigureFe(solution);
	ConfigureMakey(solution);
	ConfigureSolutionFolders(solution);

	WriteNinjaFile(solution);
	WriteMSVCSolution(solution);
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

	AccumulateProject(project, solution);

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

	AccumulateProject(project, solution);
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