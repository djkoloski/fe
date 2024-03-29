#include <FeMake/NinjaFile.h>

#include <Fe/System/Directory.h>

NinjaFile::NinjaFile(feStringView path)
{
	Directory::create(Path::dirName(path));
	_output.open(path.c_str());
}
void NinjaFile::writeComment(feStringView comment)
{
	_output << "# " << comment << "\n";
}
void NinjaFile::writeNewline()
{
	_output << "\n";
}
void NinjaFile::writeVariable(feStringView name, feStringView value)
{
	_output << name << " = " << value << "\n";
}
void NinjaFile::writeRule(const Rule &rule)
{
	_output << "rule " << rule.getName() << "\n";
	_output << "  command = " << rule.getCommand() << "\n";
	_output << "  description = " << rule.getDescription() << "\n";

	if (rule.getDeps() != "")
	{
		_output << "  deps = " << rule.getDeps() << "\n";
	}

	if (rule.getDepfile() != "")
	{
		_output << "  depfile = " << rule.getDepfile() << "\n";
	}

	if (rule.getIsGenerator())
	{
		_output << "  generator = true\n";
	}

	if (rule.getRestat())
	{
		_output << "  restat = true\n";
	}
}
void NinjaFile::writeBuild(const BuildCommand &build)
{
	_output << "build " << build.getOutputs();

	if (build.getImplicitOutputs() != "")
	{
		_output << " | " << build.getImplicitOutputs();
	}

	_output << ": " << (build.getRule() ? build.getRule()->getName() : "phony") << " " << build.getInputs();

	if (build.getImplicitDependencies() != "")
	{
		_output << " | " << build.getImplicitDependencies();
	}

	if (build.getOrderOnlyDependencies() != "")
	{
		_output << " || " << build.getOrderOnlyDependencies();
	}

	_output << "\n";
}
void NinjaFile::writeInclude(feStringView path)
{
	_output << "include " << path << "\n";
}
void NinjaFile::writeSubninja(feStringView path)
{
	_output << "subninja " << path << "\n";
}
void NinjaFile::writeDefault(feStringView targets)
{
	_output << "default " << targets << "\n";
}