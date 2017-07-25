#pragma once

#include <Fe/Container/FeString.h>
#include <Makey/BuildCommand.h>
#include <Makey/Rule.h>

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
	void writeSubninja(const feStringView path);
};