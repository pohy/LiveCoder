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

		ofxDatGui* pGui{ nullptr };
		ofxDatGuiDropdown* pDropdownShader{ nullptr };
		ofxDatGuiLabel* pLabelFps{ nullptr };
		ofxDatGuiFolder* pFolderUniforms{ nullptr };

		std::vector<ofxMidiIn> midiIns;
		void newMidiMessage(ofxMidiMessage& msg);
		std::map<string, InterpolationValue> uniformValues;
		void onTextureLoad(ofxDatGuiButtonEvent e);

		// TODO: Update pFolderUniforms
		void onShaderChange(pohy::ShaderInfo& info);
		void setupGui();
		void setupNdi();
		void setupMidi();
		void downloadShaders(string downloadDirectory = ".");

		void updateWindowTitle();
		void browseShaders(ofKeyEventArgs& key);
		void selectShaderByNumber(ofKeyEventArgs& key);
};
