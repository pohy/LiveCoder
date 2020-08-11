#pragma once

#include "ofMain.h"
#include "ofxNDI.h"
#include "ofxShader.h"
//#include "ofxDatGui.h"
#include "json.hpp"

using json = nlohmann::json;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();
		
	private:
		json config;

		ofFbo ndiFbo;
		ofxNDIsender ndiSender;

		ofxShader* pFrontShader;
		ofxShader* pBackShader;
		ofxShader shaderA;
		ofxShader shaderB;

		//ofxDatGui * pGui;
		//ofxDatGuiButton * pButton;

		void onShaderLoad(bool& e);
};
