#pragma once

#include <Fe/Container/FeString.h>

class Settings
{
public:
	enum class Compiler
	{
		Unknown,
		MSVC,
		First = MSVC,
		Last = MSVC,
	};
	enum class Platform
	{
		Unknown,
		Win_x86,
		First = Win_x86,
		Win_x64,
		Last = Win_x64,
	};
	enum class Configuration
	{
		Unknown,
		Debug,
		First = Debug,
		Release,
		Profile,
		Final,
		Last = Final,
	};

private:
	Compiler _compiler;
	Platform _platform;
	Configuration _configuration;

public:
	Settings();

	Compiler getCompiler() const;
	void setCompiler(Compiler compiler);
	feRawString getCompilerString() const;
	static feRawString getCompilerString(Compiler compiler);

	Platform getPlatform() const;
	void setPlatform(Platform platform);
	feRawString getPlatformString() const;
	static feRawString getPlatformString(Platform platform);
	feRawString getMSVCPlatformString() const;
	static feRawString getMSVCPlatformString(Platform platform);

	Configuration getConfiguration() const;
	void setConfiguration(Configuration configuration);
	feRawString getConfigurationString() const;
	static feRawString getConfigurationString(Configuration configuration);

	feString getIdentifier() const;
	feRawString getObjectFileExtension() const;
	static feRawString getObjectFileExtension(Compiler compiler);
	feRawString getExecutableFileExtension() const;
	static feRawString getExecutableFileExtension(Compiler compiler);
	feRawString getLibraryFileExtension() const;
	static feRawString getLibraryFileExtension(Compiler compiler);
	feRawString getSharedLibraryFileExtension() const;
	static feRawString getSharedLibraryFileExtension(Compiler compiler);
};

inline Settings::Compiler &operator++(Settings::Compiler &compiler)
{
	return compiler = Settings::Compiler(feWord(compiler) + 1);
}
inline Settings::Platform &operator++(Settings::Platform &platform)
{
	return platform = Settings::Platform(feWord(platform) + 1);
}
inline Settings::Configuration &operator++(Settings::Configuration &configuration)
{
	return configuration = Settings::Configuration(feWord(configuration) + 1);
}