#include <Makey/Settings.h>

Settings::Settings() :
	_compiler(Compiler::Unknown),
	_platform(Platform::Unknown),
	_configuration(Configuration::Unknown)
{}
Settings::Compiler Settings::getCompiler() const
{
	return _compiler;
}
void Settings::setCompiler(Compiler compiler)
{
	_compiler = compiler;
}
feRawString Settings::getCompilerString() const
{
	return getCompilerString(_compiler);
}
feRawString Settings::getCompilerString(Compiler compiler)
{
	switch (compiler)
	{
	case Settings::Compiler::MSVC:
		return "MSVC";
	default:
		return "Unknown";
	}
}
Settings::Platform Settings::getPlatform() const
{
	return _platform;
}
void Settings::setPlatform(Platform platform)
{
	_platform = platform;
}
feRawString Settings::getPlatformString() const
{
	return getPlatformString(_platform);
}
feRawString Settings::getPlatformString(Platform platform)
{
	switch (platform)
	{
	case Settings::Platform::Win_x86:
		return "Win_x86";
	case Settings::Platform::Win_x64:
		return "Win_x64";
	default:
		return "Unknown";
	}
}
feRawString Settings::getMSVCPlatformString() const
{
	return getMSVCPlatformString(_platform);
}
feRawString Settings::getMSVCPlatformString(Platform platform)
{
	switch (platform)
	{
	case Settings::Platform::Win_x86:
		return "Win32";
	case Settings::Platform::Win_x64:
		return "x64";
	default:
		return "Unknown";
	}
}
Settings::Configuration Settings::getConfiguration() const
{
	return _configuration;
}
void Settings::setConfiguration(Configuration configuration)
{
	_configuration = configuration;
}
feRawString Settings::getConfigurationString() const
{
	return getConfigurationString(_configuration);
}
feRawString Settings::getConfigurationString(Configuration configuration)
{
	switch (configuration)
	{
	case Settings::Configuration::Debug:
		return "Debug";
	case Settings::Configuration::Release:
		return "Release";
	case Settings::Configuration::Profile:
		return "Profile";
	case Settings::Configuration::Final:
		return "Final";
	default:
		return "Unknown";
	}
}
feString Settings::getIdentifier() const
{
	return feStringUtil::join(getPlatformString(), "_", getConfigurationString());
}
feRawString Settings::getObjectFileExtension() const
{
	return getObjectFileExtension(_compiler);
}
feRawString Settings::getObjectFileExtension(Compiler compiler)
{
	switch (compiler)
	{
	case Settings::Compiler::MSVC:
		return ".obj";
	default:
		return "";
	}
}
feRawString Settings::getExecutableFileExtension() const
{
	return getExecutableFileExtension(_compiler);
}
feRawString Settings::getExecutableFileExtension(Compiler compiler)
{
	switch (compiler)
	{
	case Settings::Compiler::MSVC:
		return ".exe";
	default:
		return "";
	}
}
feRawString Settings::getLibraryFileExtension() const
{
	return getLibraryFileExtension(_compiler);
}
feRawString Settings::getLibraryFileExtension(Compiler compiler)
{
	switch (compiler)
	{
	case Settings::Compiler::MSVC:
		return ".lib";
	default:
		return "";
	}
}