#include "Config.h"

namespace pohy
{
	using json = nlohmann::json;

	Config::Config(std::string config_path)
	{
		mConfigFile.open(config_path, ofFile::Mode::ReadWrite, false);
		if (!mConfigFile.isFile())
		{
			ofLogError("Config") << "Config(): Path does not lead to a file: " << config_path;
			return;
		}
		mConfig = json::parse(mConfigFile);
	}
}
