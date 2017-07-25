#include <Makey/Settings.h>

Settings::Settings() :
	_compiler(Settings::Compiler::Unknown),
	_platform(Settings::Platform::Unknown),
	_configuration(Settings::Configuration::Unknown)
{}
Settings::Compiler Settings::getCompiler() const
{
	return _compiler;
}
void Settings::setCompiler(Settings::Compiler compiler)
{
	_compiler = compiler;
}
const feRawString Settings::getCompilerString() const
{
	switch (_compiler)
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
void Settings::setPlatform(Settings::Platform platform)
{
	_platform = platform;
}
const feRawString Settings::getPlatformString() const
{
	switch (_platform)
	{
	case Settings::Platform::Win_x86:
		return "Win_x86";
	case Settings::Platform::Win_x64:
		return "Win_x64";
	default:
		return "Unknown";
	}
}
const feRawString Settings::getMSVCPlatformString() const
{
	switch (_platform)
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
void Settings::setConfiguration(Settings::Configuration configuration)
{
	_configuration = configuration;
}
const feRawString Settings::getConfigurationString() const
{
	switch (_configuration)
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
const feRawString Settings::getObjectFileExtension() const
{
	switch (_compiler)
	{
	case Settings::Compiler::MSVC:
		return ".obj";
	default:
		return "";
	}
}
const feRawString Settings::getExecutableFileExtension() const
{
	switch (_compiler)
	{
	case Settings::Compiler::MSVC:
		return ".exe";
	default:
		return "";
	}
}
const feRawString Settings::getLibraryFileExtension() const
{
	switch (_compiler)
	{
	case Settings::Compiler::MSVC:
		return ".lib";
	default:
		return "";
	}
}