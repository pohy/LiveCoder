#include "ofApp.h"

ofApp::ofApp(json config) {
	this->config = config;
	this->size = { config.value("width", 1280), config.value("height", 720) };
}

//--------------------------------------------------------------
void ofApp::setup() {
	//ofSetLogLevel(OF_LOG_VERBOSE);
	ofSetVerticalSync(false);

	ofEnableDepthTest();
	ofDisableArbTex();
	ofSetVerticalSync(true);
	ofSetFrameRate(config.value("fps", 60));

	auto senderName = config.at("/ndi"_json_pointer).value("senderName", "GLSL Live coder");

	ofSetWindowTitle("Sender: " + senderName);

	loadAvailableShaders();

	pGui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	pDropdownShader = pGui->addDropdown("shader", availableShaders);
	pDropdownShader->onDropdownEvent([=](ofxDatGuiDropdownEvent e) {
		shaderA.load(availableShaders[e.child]);
		shaderB.load(availableShaders[e.child]);
	});
	//pButton = pGui->addButton("Clicker");

	ndiFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	ndiSender.SetAsync();
	ndiSender.CreateSender(senderName.c_str(), size.x, size.y);

	shaderA.load(availableShaders[0]);
	shaderB.load(availableShaders[0]);
	pFrontShader = &shaderA;
	pBackShader = &shaderB;
	pFrontShader->disableWatchFiles();
	ofAddListener(pBackShader->onLoad, this, &ofApp::onShaderLoad);
}

//--------------------------------------------------------------
void ofApp::update() {
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofEnableDepthTest();

	ndiFbo.begin();
		ofClear(0);
		pFrontShader->begin();
			float time = ofGetElapsedTimef();
			pFrontShader->setUniform1f("iTime", time);
			pFrontShader->setUniform2f("iResolution", size.x, size.y);
			ofDrawRectangle(0, 0, size.x, size.y);
		pFrontShader->end();
	ndiFbo.end();

	ndiFbo.draw(0, 0, ofGetWidth(), ofGetHeight());
	ndiSender.SendImage(ndiFbo);

	ofDisableDepthTest();
	auto fpsText = to_string(ofGetFrameRate()) + "/" + to_string(ofGetTargetFrameRate());
	ofDrawBitmapString(fpsText, { 10, 10 });
}

void ofApp::exit() {
	ndiSender.ReleaseSender();
}

void ofApp::windowResized(int w, int h) {
	float aspect = 16. / 9.;
	ofSetWindowShape(w, w / aspect);
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

void ofApp::loadAvailableShaders()
{
	auto files = ofDirectory("data").getFiles();
	for (int i = 0; i < files.size(); i++) {
		auto fileName = files[i].getFileName();
		auto fragIndex = fileName.find(".frag");
		if (fragIndex == string::npos) {
			continue;
		}
		auto shaderName = fileName.substr(0, fragIndex);
		availableShaders.push_back(shaderName);
	}
}
