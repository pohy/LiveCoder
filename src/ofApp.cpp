#include "ofApp.h"
#include <vector>
#include <sstream>
#include <iostream>
#include <regex>

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

	senderName = config.at("/ndi"_json_pointer).value("senderName", "GLSL Live coder");

	loadAvailableShaders();
	auto foundShaderIt = std::find(availableShaders.begin(), availableShaders.end(), config.value("defaultShader", ""));
	currentShaderIndex = std::distance(availableShaders.begin(), foundShaderIt);

	setupGui();
	setupNdi();
	setupShader();
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
	pDropdownShader->select(currentShaderIndex);
	updateWindowTitle();
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
	}
	else if (key.key == OF_KEY_RIGHT) {
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
}

static GLSLType stringToType(string type) {
	if (type == "float") {
		return GL_FLOAT;
	}
	else if (type == "vec2") {
		return GL_FLOAT_VEC2;
	}
	else if (type == "vec3") {
		return GL_FLOAT_VEC3;
	}
	else if (type == "vec4") {
		return GL_FLOAT_VEC4;
	}
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


	pFolderUniforms->children.clear();

	auto shaderSource = pFrontShader->getShaderSource(GL_FRAGMENT_SHADER);
	//ofLogNotice("LiveCoder") << "Shader source: \n" << shaderSource;

	std::stringstream ss(shaderSource);
	string line;
	std::vector<string> lines;
	while (std::getline(ss, line, '\n')) {
		lines.push_back(line);
	}

	std::vector<std::pair<string, GLSLType>> matches;
	for (auto line : lines) {
		//ofLogNotice("LiveCoder") << "Line " << ": " << line << std::endl;
		std::smatch match;
		std::regex regex("uniform (\\w+) (\\w+);");

		if (!std::regex_match(line, match, regex) || match.size() < 2) {
			continue;
		}
		auto type = match[1];
		GLSLType uniformType = stringToType(match[1]);
		if (uniformType == NULL) {
			ofLogError("LiveCoder") << "Unknown uniform type: " << match[1];
			continue;
		}
		matches.push_back(std::make_pair(match[2], uniformType));
	}
	for (auto match : matches) {
		ofLogNotice("LiveCoder") << "Uniform match: " << match.first << endl;
		switch (match.second)
		{
		case GL_FLOAT:
			pFolderUniforms->addSlider(match.first, 0, 1);
			break;
		default:
			break;
		}
	}

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

void ofApp::setupGui() {
	pGui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	pLabelFps = pGui->addLabel("fps");
	pDropdownShader = pGui->addDropdown("shader", availableShaders);
	pDropdownShader->select(currentShaderIndex);
	pDropdownShader->onDropdownEvent([=](ofxDatGuiDropdownEvent e) {
		loadShader(e.child);
		});
	pFolderUniforms = pGui->addFolder("Uniforms");
	pFolderUniforms->expand();
}

void ofApp::setupNdi() {
	ndiFbo.allocate(renderSize.x, renderSize.y, GL_RGBA);
	ndiSender.SetAsync();
	ndiSender.CreateSender(senderName.c_str(), renderSize.x, renderSize.y);
}

void ofApp::setupShader() {
	if (availableShaders.size() > 0) {
		loadShader(currentShaderIndex);
	}
	pFrontShader = &shaderA;
	pBackShader = &shaderB;
	pFrontShader->disableWatchFiles();
	ofAddListener(pBackShader->onLoad, this, &ofApp::onShaderLoad);
}

void ofApp::updateWindowTitle()
{
	auto shaderName = availableShaders.size() > 0 ? availableShaders[currentShaderIndex] : "";
	ofSetWindowTitle("Sender: " + senderName + " / Shader: " + shaderName);
}
