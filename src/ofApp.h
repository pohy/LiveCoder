#pragma once

#include "ofMain.h"
#include "ofxNDI.h"
#include "ofxShader.h"
#include "json.hpp"
#include "ofxDatGui.h"

using json = nlohmann::json;

typedef int GLSLType;

class ofApp : public ofBaseApp{

	public:
		ofApp(json config);
		void setup();
		void update();
		void draw();
		void exit();
		void loadShader(int index);
		void windowResized(int w, int h);
		void keyPressed( ofKeyEventArgs & key );

		glm::vec2 renderSize;
		
	private:
		json config;

		ofFbo ndiFbo;
		string senderName;
		ofxNDIsender ndiSender;

		ofxShader* pFrontShader;
		ofxShader* pBackShader;
		ofxShader shaderA;
		ofxShader shaderB;

		ofxDatGui* pGui;
		ofxDatGuiDropdown* pDropdownShader;
		ofxDatGuiLabel* pLabelFps;
		ofxDatGuiFolder* pFolderUniforms;

		std::vector<string> availableShaders;
		int currentShaderIndex;

		void onShaderLoad(bool& e);
		void loadAvailableShaders();
		void setupGui();
		void setupNdi();
		void setupShader();

		void updateWindowTitle();
		void browseShaders(ofKeyEventArgs& key);
};
