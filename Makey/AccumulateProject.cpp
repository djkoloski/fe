#include <Makey/AccumulateProject.h>

#include <Fe/System/Directory.h>

static feString GetBinaryPath(const Project &project, const Solution &solution)
{
	return Path::join(
		"$binDir",
		project.getName(),
		Path::addExtension(
			project.getName(),
			solution.getSettings().getExecutableFileExtension()));
}

static feString GetLibraryPath(const Project &project, const Solution &solution)
{
	return Path::join(
			"$binDir",
			project.getName(),
			Path::addExtension(
				project.getName(),
				solution.getSettings().getLibraryFileExtension()));
}

void AccumulateProject(Project &project, Solution &solution)
{
	auto sourceDir = project.getName();

	// Get rules
	auto *compile = solution.getRule("compile");
	auto *copy = solution.getRule("copy");
	auto *codegen = solution.getRule("codegen");

	// Collect files and add .h and .cpp build rules
	auto allHeaders = feString();
	auto allObjects = feString();

	auto mainObject = feString();

	Directory::iterate(
		sourceDir,
		[compile = compile, copy = copy, codegen = codegen, &solution = solution, &project = project, &allHeaders = allHeaders, &allObjects = allObjects, &sourceDir = sourceDir, &mainObject = mainObject](const FileInfo &fileInfo)
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
					if (project.getCodegenEnabled())
					{
						build.setRule(codegen);
						build.setImplicitDependencies(solution.getProject("CGen")->getBuildAlias().getInputs());
					}
					else
					{
						build.setRule(copy);
					}
					build.setInputs(header);
					build.setOutputs(genHeader);
					allHeaders = feStringUtil::append(allHeaders, genHeader);

					if (project.getCodegenEnabled())
					{
						auto source = Path::join("$buildDir", Path::addExtension(Path::removeExtension(relPath) + "-cgen", ".cpp"));
						build.setImplicitOutputs(source);
						auto object = Path::addExtension(Path::removeExtension(source), solution.getSettings().getObjectFileExtension());
						auto &sourceBuild = project.addBuildCommand();
						sourceBuild.setRule(compile);
						sourceBuild.setInputs(source);
						sourceBuild.setOutputs(object);
						sourceBuild.setImplicitDependencies(genHeader);
						allObjects = feStringUtil::append(allObjects, object);
					}
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

					if (Path::fileName(relPath) == "Main")
					{
						mainObject = object;
					}

					allObjects = feStringUtil::append(allObjects, object);
				}
				else if (extension != ".vcxproj" && extension != ".vcxproj.filters" && extension != ".vcxproj.user")
				{
					project.addOtherFilePath(relPath);
				}
			}
		},
		true);

	auto &codegenPhase = project.addBuildCommand();
	codegenPhase.setRule(null);
	codegenPhase.setInputs(allHeaders);
	codegenPhase.setOutputs(project.getName() + "_codegen");

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

	auto &targetCommand = project.addBuildCommand();
	auto targetInputs = feString();
	auto targetOutputPath = feString();
	switch (project.getType())
	{
	case Project::Type::Executable:
		targetCommand.setRule(solution.getRule("link"));
		targetInputs = allObjects;
		targetOutputPath = GetBinaryPath(project, solution);
		break;
	case Project::Type::Library:
		targetCommand.setRule(solution.getRule("lib"));
		targetInputs = allObjects.erase(allObjects.find(mainObject), mainObject.length());
		targetOutputPath = GetLibraryPath(project, solution);
		break;
	default:
		FE_ERROR_SWITCH_VALUE();
		break;
	}

	targetCommand.setInputs(targetInputs);
	targetCommand.setOutputs(targetOutputPath);
	targetCommand.setImplicitDependencies(libraryDependencies);

	auto unitTestInputPath = feString();
	switch (project.getType())
	{
	case Project::Type::Executable:
	{
		unitTestInputPath = targetOutputPath;
		break;
	}
	case Project::Type::Library:
	{
		// Unit test our libraries with a canary exectuable
		auto &canaryCommand = project.addBuildCommand();
		canaryCommand.setRule(solution.getRule("linkWholeArchive"));
		canaryCommand.setInputs(feStringUtil::append(targetOutputPath, mainObject));

		auto canaryOutputPath = GetBinaryPath(project, solution);
		canaryCommand.setOutputs(canaryOutputPath);

		unitTestInputPath = canaryOutputPath;
		break;
	}
	default:
		FE_ERROR_SWITCH_VALUE();
		break;
	}

	auto &alias = project.getBuildAlias();
	alias.setRule(solution.getRule("unitTest"));

	alias.setInputs(unitTestInputPath);
	alias.setOutputs(project.getName());
}