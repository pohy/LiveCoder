#pragma once
#include <string>
#include "ofMain.h"
#include "json.hpp"

namespace pohy
{
	using json = nlohmann::json;

	class Config
	{
	public:
		explicit Config(std::string config_path);
		json mConfig;

	private:
		ofFile mConfigFile;
	};
}
