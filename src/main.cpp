#include "json.hpp"
#include "ofMain.h"
#include "ofApp.h"

using json = nlohmann::json;

//========================================================================
int main() {
	auto config = json::parse(ofBufferFromFile("config.json", false).getText());
	auto windowConfig = config.at("window");

	ofGLFWWindowSettings settings;
	settings.setGLVersion(3, 3);
	settings.setSize(windowConfig.value("width", 1280), windowConfig.value("height", 720));

	ofCreateWindow(settings);			// <-------- setup the GL context

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp(new ofApp(config));

}
