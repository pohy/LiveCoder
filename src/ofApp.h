#pragma once

#include "ofMain.h"
#include "ofxNDI.h"
#include "ofxShader.h"
#include "ofxDatGui.h"
#include "json.hpp"

using json = nlohmann::json;

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
		void exit();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void mouseEntered(int x, int y);
		void mouseExited(int x, int y);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
	private:
		json config;

		ofFbo ndiFbo;
		ofxNDIsender ndiSender;

		ofxShader shader;
		ofBoxPrimitive box;

		ofxDatGui * pGui;
		ofxDatGuiButton * pButton;
};
