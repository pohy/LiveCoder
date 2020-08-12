#pragma once

#include "ofMain.h"
#include "ofxNDI.h"
#include "ofxShader.h"
#include "json.hpp"
#include "ofxDatGui.h"

using json = nlohmann::json;

class ofApp : public ofBaseApp{

	public:
		ofApp(json config);
		void setup();
		void update();
		void draw();
		void exit();
		void windowResized(int w, int h);

		glm::vec2 renderSize;
		
	private:
		json config;

		ofFbo ndiFbo;
		ofxNDIsender ndiSender;

		ofxShader* pFrontShader;
		ofxShader* pBackShader;
		ofxShader shaderA;
		ofxShader shaderB;

		ofxDatGui* pGui;
		ofxDatGuiDropdown* pDropdownShader;

		std::vector<string> availableShaders;
		//ofxDatGuiButton * pButton;

		void onShaderLoad(bool& e);
		void loadAvailableShaders();
};
