#include <FeMake/Project.h>

#include <Fe/System/Directory.h>

#include <FeMake/Solution.h>

Project::Project(feStringView name, const feGUID &visualStudioGUID) :
	_name(name),
	_visualStudioGUID(visualStudioGUID),
	_codegenEnabled(true)
{}
const feString &Project::getName() const
{
	return _name;
}
const feGUID &Project::getVisualStudioGUID() const
{
	return _visualStudioGUID;
}
Project::Type Project::getType() const
{
	return _type;
}
void Project::setType(Type type)
{
	_type = type;
}
const feVector<BuildCommand> &Project::getBuildCommands() const
{
	return _buildCommands;
}
BuildCommand &Project::addBuildCommand()
{
	_buildCommands.emplace_back();
	return _buildCommands.back();
}
BuildCommand &Project::getBuildAlias()
{
	return _buildAlias;
}
const BuildCommand &Project::getBuildAlias() const
{
	return _buildAlias;
}
void Project::addModule(const Module *module)
{
	_modules.push_back(module);
}
const feVector<const Module *> &Project::getModules() const
{
	return _modules;
}
void Project::addHeaderFilePath(feStringView headerFilePath)
{
	_headerFilePaths.push_back(headerFilePath);
}
const feVector<feString> &Project::getHeaderFilePaths() const
{
	return _headerFilePaths;
}
void Project::addInlineFilePath(feStringView otherFilePath)
{
	_inlineFilePaths.push_back(otherFilePath);
}
const feVector<feString> &Project::getInlineFilePaths() const
{
	return _inlineFilePaths;
}
void Project::addSourceFilePath(feStringView sourceFilePath)
{
	_sourceFilePaths.push_back(sourceFilePath);
}
const feVector<feString> &Project::getSourceFilePaths() const
{
	return _sourceFilePaths;
}
void Project::addOtherFilePath(feStringView otherFilePath)
{
	_otherFilePaths.push_back(otherFilePath);
}
const feVector<feString> &Project::getOtherFilePaths() const
{
	return _otherFilePaths;
}
void Project::setCodegenEnabled(feBool codegenEnabled)
{
	_codegenEnabled = codegenEnabled;
}
feBool Project::getCodegenEnabled() const
{
	return _codegenEnabled;
}
void Project::collectDependentModules(feHashTable<feString, const Module *> &modules) const
{
	for (const auto *module : _modules)
	{
		if (modules.count(module->getName()) == 0)
		{
			modules[module->getName()] = module;
		}
		for (const auto *dependency : module->getDependencies())
		{
			dependency->collectDependentModules(modules);
		}
	}
}
const feString &Project::getVisualStudioDebuggerDefaultArguments() const
{
	return _visualStudioDebuggerDefaultArguments;
}
void Project::setVisualStudioDebuggerDefaultArguments(feStringView arguments)
{
	_visualStudioDebuggerDefaultArguments = arguments;
}
void Project::collectFiles()
{
	Directory::iterate(
		_name,
		[this](const FileInfo &fileInfo)
		{
			const auto &relPath = Path::removePrefix(fileInfo.getPath(), _name);
			auto extension = Path::extension(relPath);

			if (fileInfo.getType() == FileType::File)
			{
				if (extension == ".h")
				{
					addHeaderFilePath(relPath);
				}
				else if (extension == ".inl")
				{
					addInlineFilePath(relPath);
				}
				else if (extension == ".cpp")
				{
					addSourceFilePath(relPath);
				}
				else if (
					extension != ".vcxproj"
					&& extension != ".vcxproj.filters"
					&& extension != ".vcxproj.user")
				{
					addOtherFilePath(relPath);
				}
			}
		},
		true);
}
void Project::makeBuildRules(const Solution &solution)
{
	auto buildHeaders = feString();
	auto libObjects = feString();
	auto mainObject = feString();
	auto executablePath = feString();

	// Header files
	for (const auto &path : _headerFilePaths)
	{
		if (_codegenEnabled)
		{
			makeCodegenBuildRules(path, solution, buildHeaders, libObjects);
		}
		else
		{
			buildHeaders = feStringUtil::append(
				buildHeaders,
				makeHeaderCopyBuildRule(path, solution));
		}
	}

	// Inline files
	for (const auto &path : _inlineFilePaths)
	{
		buildHeaders = feStringUtil::append(
			buildHeaders,
			makeHeaderCopyBuildRule(path, solution));
	}

	// Source files
	for (const auto &path : _sourceFilePaths)
	{
		auto objectPath = makeSourceBuildRule(path, solution);
		if (Path::fileName(path) == "Main")
		{
			mainObject = objectPath;
		}
		else
		{
			libObjects = feStringUtil::append(
				libObjects,
				objectPath);
		}
	}

	// Build headers
	makeBuildHeadersBuildRule(buildHeaders, solution);

	// Library dependencies
	auto libraryDependencies = makeLibraryDependencyBuildRules(solution);

	// Library and executable
	switch (_type)
	{
	case Type::Executable:
		executablePath = makeExecutableBuildRule(feStringUtil::append(libObjects, mainObject), libraryDependencies, solution, false);
		break;
	case Type::Library:
	{
		auto libraryPath = makeLibraryBuildRule(libObjects, solution);
		executablePath = makeExecutableBuildRule(feStringUtil::append(mainObject, libraryPath), libraryDependencies, solution, true);
		break;
	}
	default:
		FE_ERROR_SWITCH_VALUE();
		break;
	}

	// Unit test and alias
	makeUnitTestBuildRule(executablePath, solution);
}
void Project::makeCodegenBuildRules(feStringView path, const Solution &solution, feString &buildHeaders, feString &libObjects)
{
	auto sourceHeaderPath = Path::join("$sourceDir", path);
	auto buildHeaderPath = Path::join("$buildDir", path);
	auto sourcePath = Path::join("$buildDir", Path::addExtension(Path::removeExtension(path) + "-fegen", ".cpp"));
	auto objectPath = Path::addExtension(Path::removeExtension(sourcePath), solution.getSettings().getObjectFileExtension());

	auto &headerBuild = addBuildCommand();
	headerBuild.setRule(solution.getRule("codegen"));
	headerBuild.setImplicitDependencies(solution.getProject("FeGen")->getBuildAlias().getInputs());
	headerBuild.setInputs(sourceHeaderPath);
	headerBuild.setOutputs(buildHeaderPath);
	headerBuild.setImplicitOutputs(sourcePath);
	buildHeaders = feStringUtil::append(
		buildHeaders,
		buildHeaderPath);

	auto &sourceBuild = addBuildCommand();
	sourceBuild.setRule(solution.getRule("compile"));
	sourceBuild.setInputs(sourcePath);
	sourceBuild.setOutputs(objectPath);
	sourceBuild.setImplicitDependencies(buildHeaderPath);
	libObjects = feStringUtil::append(
		libObjects,
		objectPath);
}
feString Project::makeHeaderCopyBuildRule(feStringView path, const Solution &solution)
{
	auto sourceHeader = Path::join("$sourceDir", path);
	auto buildHeader = Path::join("$buildDir", path);

	auto &build = addBuildCommand();
	build.setRule(solution.getRule("copy"));
	build.setInputs(sourceHeader);
	build.setOutputs(buildHeader);

	return buildHeader;
}
feString Project::makeSourceBuildRule(feStringView path, const Solution &solution)
{
	auto source = Path::join("$sourceDir", path);
	auto object = Path::join("$buildDir", Path::addExtension(Path::removeExtension(path), solution.getSettings().getObjectFileExtension()));

	auto &build = addBuildCommand();
	build.setRule(solution.getRule("compile"));
	build.setInputs(source);
	build.setOutputs(object);
	build.setOrderOnlyDependencies(_name + "_headers");

	return object;
}
void Project::makeBuildHeadersBuildRule(feStringView buildHeaders, const Solution &solution)
{
	auto buildHeadersDependencies = feString();
	for (auto *module : _modules)
	{
		for (auto *dependency : module->getDependencies())
		{
			buildHeadersDependencies = feStringUtil::append(
				buildHeadersDependencies,
				dependency->getName() + "_headers");
		}
	}

	auto &build = addBuildCommand();
	build.setRule(null);
	build.setInputs(buildHeaders);
	build.setOutputs(_name + "_headers");
}
feString Project::makeLibraryDependencyBuildRules(const Solution &solution)
{
	auto libraryDependencies = feString();

	auto modules = feHashTable<feString, const Module *>();
	collectDependentModules(modules);
	for (const auto &pair : modules)
	{
		const auto &module = *pair.second;

		// Static library dependencies
		libraryDependencies = feStringUtil::append(
			libraryDependencies,
			feStringUtil::joinRangeWrapped(
				"",
				"",
				" ",
				module.getLibs().begin(),
				module.getLibs().end()));

		// Shared library dependencies
		for (const auto &sharedLibPath : module.getSharedLibs())
		{
			auto destPath = Path::join(
				"$binDir",
				_name,
				Path::baseName(sharedLibPath));

			auto &copySharedLib = addBuildCommand();
			copySharedLib.setRule(solution.getRule("copy"));
			copySharedLib.setInputs(sharedLibPath);
			copySharedLib.setOutputs(destPath);

			libraryDependencies = feStringUtil::append(
				libraryDependencies,
				destPath);
		}
	}

	return libraryDependencies;
}
feString Project::makeLibraryBuildRule(feStringView objects, const Solution &solution)
{
	auto outputPath = Path::join(
		"$binDir",
		_name,
		Path::addExtension(
			_name,
			solution.getSettings().getLibraryFileExtension()));

	auto &build = addBuildCommand();
	build.setRule(solution.getRule("lib"));
	build.setInputs(objects);
	build.setOutputs(outputPath);

	return outputPath;
}
feString Project::makeExecutableBuildRule(feStringView objects, feStringView libraryDependencies, const Solution &solution, feBool wholeArchive)
{
	auto outputPath = Path::join(
		"$binDir",
		_name,
		Path::addExtension(
			_name,
			solution.getSettings().getExecutableFileExtension()));
	auto *rule = solution.getRule((wholeArchive ? "linkWholeArchive" : "link"));

	auto &build = addBuildCommand();
	build.setRule(rule);
	build.setInputs(objects);
	build.setImplicitDependencies(libraryDependencies);
	build.setOutputs(outputPath);

	return outputPath;
}
void Project::makeUnitTestBuildRule(feStringView executablePath, const Solution &solution)
{
	_buildAlias.setRule(solution.getRule("unitTest"));
	_buildAlias.setInputs(executablePath);
	_buildAlias.setOutputs(_name);
}