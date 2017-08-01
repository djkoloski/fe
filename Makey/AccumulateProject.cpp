#include <Makey/AccumulateProject.h>

#include <Fe/System/Directory.h>

void AccumulateProject(Project &project, Solution &solution)
{
	auto sourceDir = project.getName();

	// Collect files and add .h and .cpp build rules
	auto allHeaders = feString();
	auto allObjects = feString();
	Directory::iterate(
		sourceDir,
		[compile = solution.getRule("compile"), cgen = solution.getRule("cgen"), copy = solution.getRule("copy"), &solution = solution, &project = project, &allHeaders = allHeaders, &allObjects = allObjects, &sourceDir = sourceDir](const FileInfo &fileInfo)
		{
			const auto &relPath = Path::removePrefix(fileInfo.getPath(), sourceDir);
			feString extension = Path::extension(relPath);

			if (fileInfo.getType() == FileType::File)
			{
				if (extension == ".h")
				{
					project.addHeaderFilePath(relPath);

					auto header = Path::join("$sourceDir", relPath);
					auto genHeader = Path::join("$buildDir", relPath);
					auto &build = project.addBuildCommand();
					build.setRule(cgen);
					build.setInputs(header);
					build.setOutputs(genHeader);
					allHeaders = feStringUtil::append(allHeaders, genHeader);
				}
				else if (extension == ".inl")
				{
					project.addOtherFilePath(relPath);

					auto header = Path::join("$sourceDir", relPath);
					auto genHeader = Path::join("$buildDir", relPath);
					auto &build = project.addBuildCommand();
					build.setRule(copy);
					build.setInputs(header);
					build.setOutputs(genHeader);
					allHeaders = feStringUtil::append(allHeaders, genHeader);
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
					build.setOrderOnlyDependencies(project.getName() + "_codegen");
					allObjects = feStringUtil::append(allObjects, object);
				}
				else if (extension != ".vcxproj" && extension != ".vcxproj.filters" && extension != ".vcxproj.user")
				{
					project.addOtherFilePath(relPath);
				}
			}
		},
		true);

	auto &codegen = project.addBuildCommand();
	codegen.setRule(null);
	codegen.setInputs(allHeaders);
	codegen.setOutputs(project.getName() + "_codegen");

	auto modules = feHashTable<feString, const Module *>();
	project.collectDependentModules(modules);

	auto libraryDependencies = feString();
	for (const auto &pair : modules)
	{
		const auto &module = *pair.second;

		// Add library dependencies
		libraryDependencies = feStringUtil::append(
			libraryDependencies,
			feStringUtil::joinRangeWrapped(
				"",
				"",
				" ",
				module.getLibs().begin(),
				module.getLibs().end()));

		// Copy shared libraries to the bin directory
		for (const auto &sharedLibPath : module.getSharedLibs())
		{
			auto destPath = Path::join(
				"$binDir",
				project.getName(),
				Path::baseName(sharedLibPath));

			auto &copySharedLibrary = project.addBuildCommand();
			copySharedLibrary.setRule(solution.getRule("copy"));
			copySharedLibrary.setInputs(sharedLibPath);
			copySharedLibrary.setOutputs(destPath);

			libraryDependencies = feStringUtil::append(
				libraryDependencies,
				destPath);
		}
	}

	auto binaryPath = feString();
	switch (project.getType())
	{
	case Project::Type::Executable:
		binaryPath = Path::join(
			"$binDir",
			project.getName(),
			Path::addExtension(
				project.getName(),
				solution.getSettings().getExecutableFileExtension()));
		break;
	case Project::Type::Library:
		binaryPath = Path::join(
			"$binDir",
			project.getName(),
			Path::addExtension(
				project.getName(),
				solution.getSettings().getLibraryFileExtension()));
		break;
	default:
		FE_ERROR_SWITCH_VALUE();
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
		FE_ERROR_SWITCH_VALUE();
		break;
	}
	combine.setInputs(allObjects);
	combine.setOutputs(binaryPath);
	combine.setImplicitDependencies(libraryDependencies);

	auto &alias = project.getBuildAlias();
	switch (project.getType())
	{
	case Project::Type::Executable:
		alias.setRule(solution.getRule("unitTest"));
		break;
	case Project::Type::Library:
		// TODO: unit test a canary project
		alias.setRule(null);
		break;
	default:
		FE_ERROR_SWITCH_VALUE();
		break;
	}
	alias.setInputs(binaryPath);
	alias.setOutputs(project.getName());
}