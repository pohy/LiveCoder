#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	config = json::parse(ofBufferFromFile("config.json", false).getText());

	ofEnableDepthTest();
	ofDisableArbTex();
	ofSetVerticalSync(true);
	ofSetFrameRate(config.value("fps", 60));

	auto senderName = config.at("/ndi"_json_pointer).value("senderName", "GLSL Live coder");

	ofSetWindowTitle("Sender: " + senderName);

	//pGui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	//pButton = pGui->addButton("Clicker");

	ndiFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	ndiSender.SetAsync();
	ndiSender.CreateSender(senderName.c_str(), ofGetWidth(), ofGetHeight());

	shaderA.load("shader");
	shaderB.load("shader");
	pFrontShader = &shaderA;
	pBackShader = &shaderB;
	pFrontShader->disableWatchFiles();
	ofAddListener(pBackShader->onLoad, this, &ofApp::onShaderLoad);
}

//--------------------------------------------------------------
void ofApp::update() {
	//pGui->update();
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofEnableDepthTest();

	ndiFbo.begin();
		ofClear(0);
		pFrontShader->begin();
			float time = ofGetElapsedTimef();// ->getMouseDown() ? 0 : ofGetElapsedTimef();
			pFrontShader->setUniform1f("iTime", time);
			pFrontShader->setUniform2f("iResolution", ofGetWidth(), ofGetHeight());
			ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
		pFrontShader->end();
	ndiFbo.end();

	ndiFbo.draw(0, 0);
	ndiSender.SendImage(ndiFbo);

	ofDisableDepthTest();
	//pGui->draw();
	auto fpsText = to_string(ofGetFrameRate()) + "/" + to_string(ofGetTargetFrameRate());
	ofDrawBitmapString(fpsText, { 10, 10 });
}

void ofApp::exit() {
	ndiSender.ReleaseSender();
}

void ofApp::onShaderLoad(bool& e) {
	if (!e) {
		return;
	}

	//Swap front and back shaders
	auto pBackShaderOld = pBackShader;
	pBackShader = pFrontShader;
	pFrontShader = pBackShaderOld;
	pBackShader->enableWatchFiles();
	pFrontShader->disableWatchFiles();
	ofRemoveListener(pBackShaderOld->onLoad, this, &ofApp::onShaderLoad);
	ofAddListener(pBackShader->onLoad, this, &ofApp::onShaderLoad);
}
