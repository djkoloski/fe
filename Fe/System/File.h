#pragma once

#include <Fe/Container/FeString.h>
#include <Fe/Core/Core.h>

class File
{
public:
	static feStatus File::readAllBytes(feStringView path, feChar *&chars, feUWord &charsRead);
};