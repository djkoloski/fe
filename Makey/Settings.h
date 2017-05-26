#pragma once

#include <Fe/Container/FeString.h>

class Settings
{
public:
	enum class Compiler
	{
		Unknown,
		MSVC,
	};
	enum class Platform
	{
		Unknown,
		Win_x86,
		Win_x64,
	};
	enum class Configuration
	{
		Unknown,
		Debug,
		Release,
		Profile,
		Final,
	};

private:
	Compiler _compiler;
	Platform _platform;
	Configuration _configuration;
	
public:
	Settings();
	
	Compiler getCompiler() const;
	void setCompiler(Compiler compiler);
	const feRawString getCompilerString() const;
	
	Platform getPlatform() const;
	void setPlatform(Platform platform);
	const feRawString getPlatformString() const;
	
	Configuration getConfiguration() const;
	void setConfiguration(Configuration configuration);
	const feRawString getConfigurationString() const;
	
	feString getIdentifier() const;
	const feRawString getObjectFileExtension() const;
	const feRawString getExecutableFileExtension() const;
	const feRawString getLibraryFileExtension() const;
};