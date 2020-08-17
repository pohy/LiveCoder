#pragma once

#include "ofMain.h"
#include "ofxNDI.h"
#include "ofxShader.h"
#include "json.hpp"
#include "ofxDatGui.h"
#include "ofxMidi.h"
#include "Shader.h"

using json = nlohmann::json;

typedef struct {
	float current;
	float target;
} InterpolationValue;

class ofApp : public ofBaseApp, public ofxMidiListener {

	public:
		ofApp(json config);
		void setup();
		void update();
		void draw();
		void exit();
		void windowResized(int w, int h);
		void keyPressed( ofKeyEventArgs & key );

		glm::vec2 renderSize;
		
	private:
		json config;

		ofFbo ndiFbo;
		string senderName;
		ofxNDIsender ndiSender;
		
		pohy::LiveShader shader;

		ofxDatGui* pGui;
		ofxDatGuiDropdown* pDropdownShader;
		ofxDatGuiLabel* pLabelFps;
		ofxDatGuiFolder* pFolderUniforms;

		std::vector<ofxMidiIn> midiIns;
		void newMidiMessage(ofxMidiMessage& msg);
		std::map<string, InterpolationValue> uniformValues;

		// TODO: Update pFolderUniforms
		void onShaderChange(pohy::ShaderInfo& info);
		void setupGui();
		void setupNdi();
		void setupMidi();

		void updateWindowTitle();
		void browseShaders(ofKeyEventArgs& key);
		void selectShaderByNumber(ofKeyEventArgs& key);
};
