#pragma once

#include <Fe/Container/FeString.h>
#include <FeMake/BuildCommand.h>
#include <FeMake/Rule.h>

#include <ostream>
#include <fstream>

class NinjaFile
{
private:
	std::ofstream _output;

public:
	NinjaFile(feStringView path);

	void writeComment(feStringView comment);
	void writeNewline();
	void writeVariable(feStringView name, feStringView value);
	void writeRule(const Rule &rule);
	void writeBuild(const BuildCommand &build);
	void writeInclude(feStringView path);
	void writeSubninja(feStringView path);
	void writeDefault(feStringView targets);
};