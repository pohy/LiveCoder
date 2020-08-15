#pragma once

#include "ofMain.h"
#include "ofxNDI.h"
#include "ofxShader.h"
#include "json.hpp"
#include "ofxDatGui.h"
#include "ofxMidi.h"

using json = nlohmann::json;

typedef int GLSLType;
typedef std::pair<string, GLSLType> Uniform;

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

		std::vector<ofxMidiIn> midiIns;
		void newMidiMessage(ofxMidiMessage& msg);
		std::map<string, InterpolationValue> uniformValues;

		std::vector<string> availableShaders;
		int currentShaderIndex;
		std::vector<Uniform> uniforms;

		void onShaderLoad(bool& e);
		void loadAvailableShaders();
		void setupGui();
		void setupNdi();
		void setupShader();
		void setupMidi();
		void parseUniforms();

		void updateWindowTitle();
		void browseShaders(ofKeyEventArgs& key);
		void selectShaderByNumber(ofKeyEventArgs& key);
};
