#include <Makey/AccumulateProject.h>

#include <Fe/System/Directory.h>

void AccumulateProject(Project &project, Solution &solution)
{
	auto sourceDir = project.getName();

	auto allObjects = feString();
	Directory::iterate(
		sourceDir,
		[compile = solution.getRule("compile"), &solution = solution, &project = project, &allObjects = allObjects, &sourceDir = sourceDir](const FileInfo &fileInfo)
		{
			const auto &relPath = Path::removePrefix(fileInfo.getPath(), sourceDir);
			feString extension = Path::extension(relPath);

			if (fileInfo.getType() == FileType::File)
			{
				if (extension == ".h")
				{
					project.addHeaderFilePath(relPath);
				}
				else if (extension == ".cpp")
				{
					project.addSourceFilePath(relPath);

					auto source = Path::join("$sourceDir", relPath);
					auto object = Path::join("$buildDir", Path::addExtension(Path::removeExtension(relPath), solution.getSettings().getObjectFileExtension()));
					auto &build = project.addBuildCommand();
					build.setRule(compile);
					build.setInputs(source);
					build.setOutputs(object);
					allObjects = feStringUtil::append(allObjects, object);
				}
				else if (extension != ".vcxproj" && extension != ".vcxproj.filters" && extension != ".vcxproj.user")
				{
					project.addOtherFilePath(relPath);
				}
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