#include "ofApp.h"
#include <sstream>

ofApp::ofApp(json config) {
	auto renderConfig = config.at("render");
	this->config = config;
	this->renderSize = { renderConfig.value("width", 1280), renderConfig.value("height", 720) };
	this->currentShaderIndex = 0;
}

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(false);

	ofEnableDepthTest();
	ofDisableArbTex();
	ofSetVerticalSync(true);
	ofSetFrameRate(config.value("fps", 60));

	auto senderName = config.at("/ndi"_json_pointer).value("senderName", "GLSL Live coder");

	ofSetWindowTitle("Sender: " + senderName);

	loadAvailableShaders();
	auto foundShaderIt = std::find(availableShaders.begin(), availableShaders.end(), config.value("defaultShader", ""));
	currentShaderIndex = std::distance(availableShaders.begin(), foundShaderIt);

	pGui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	pLabelFps = pGui->addLabel("fps");
	pDropdownShader = pGui->addDropdown("shader", availableShaders);
	pDropdownShader->select(currentShaderIndex);
	pDropdownShader->onDropdownEvent([=](ofxDatGuiDropdownEvent e) {
		loadShader(e.child);
		});

	ndiFbo.allocate(renderSize.x, renderSize.y, GL_RGBA);
	ndiSender.SetAsync();
	ndiSender.CreateSender(senderName.c_str(), renderSize.x, renderSize.y);

	if (availableShaders.size() > 0) {
		loadShader(currentShaderIndex);
	}
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
	pFrontShader->setUniform2f("iResolution", renderSize.x, renderSize.y);
	ofDrawRectangle(0, 0, renderSize.x, renderSize.y);
	pFrontShader->end();
	ndiFbo.end();

	ndiFbo.draw(0, 0, ofGetWidth(), ofGetHeight());
	ndiSender.SendImage(ndiFbo);

	ofDisableDepthTest();
	stringstream fpsText;
	fpsText << "FPS: ";
	fpsText << fixed << setprecision(2) << ofGetFrameRate();
	pLabelFps->setLabel(fpsText.str());
}

void ofApp::exit() {
	ndiSender.ReleaseSender();
}

void ofApp::loadShader(int index) {
	if (index < 0 || index > availableShaders.size() - 1) {
		ofLogError("LiveCoder") << "loadShader(): Shader index out of bounds: " << index;
		return;
	}
	currentShaderIndex = index;
	shaderA.load(availableShaders[currentShaderIndex]);
	shaderB.load(availableShaders[currentShaderIndex]);
}

void ofApp::windowResized(int w, int h) {
	float aspect = 16. / 9.;
	ofSetWindowShape(w, w / aspect);
}

void ofApp::keyPressed(ofKeyEventArgs& key)
{
	if (key.key != OF_KEY_LEFT && key.key != OF_KEY_RIGHT) {
		return;
	}
	int increment = 0;
	if (key.key == OF_KEY_LEFT) {
		increment = -1;
	} else if (key.key == OF_KEY_RIGHT) {
		increment = 1;
	}
	currentShaderIndex += increment;
	int maxIndex = availableShaders.size() - 1;
	if (currentShaderIndex > maxIndex) {
		currentShaderIndex = 0;
	}
	else if (currentShaderIndex < 0) {
		currentShaderIndex = maxIndex;
	}
	loadShader(currentShaderIndex);
	pDropdownShader->select(currentShaderIndex);
}

void ofApp::onShaderLoad(bool& e) {
	if (!e) {
		return;
	}
	ofLogNotice("LiveCoder") << "onShaderLoad(): Reloading shader";

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
	auto files = ofDirectory(".").getFiles();
	for (int i = 0; i < files.size(); i++) {
		auto fileName = files[i].getFileName();
		auto fragIndex = fileName.find(".frag");
		if (fragIndex == string::npos || fileName[0] == '.') {
			continue;
		}
		auto shaderName = fileName.substr(0, fragIndex);
		availableShaders.push_back(shaderName);
	}
}
