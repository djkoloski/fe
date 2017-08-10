#pragma once

#include <FeMake/Solution.h>

void WriteNinjaFile(const Solution &solution);
void WriteMSVCSolution(const Solution &solution);
void WriteMSVCProject(const Project &project, const Solution &solution);
void WriteGitIntegration();