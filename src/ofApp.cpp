#include "ofApp.h"
#include <vector>
#include <sstream>
#include <iostream>

static float lerp(float current, float target, float t) {
	return (1 - t) * current + t * target;
}

ofApp::ofApp(json config) {
	auto renderConfig = config.at("render");
	this->config = config;
	this->renderSize = { renderConfig.value("width", 1280), renderConfig.value("height", 720) };
}

//--------------------------------------------------------------
void ofApp::setup() {
	ofSetVerticalSync(false);

	ofEnableDepthTest();
	ofDisableArbTex();
	ofSetVerticalSync(true);
	ofSetFrameRate(config.value("fps", 60));

	senderName = config.at("/ndi"_json_pointer).value("senderName", "GLSL Live coder");

	shader.load();
	setupGui();
	setupNdi();
	setupMidi();

	auto availableShaders = shader.getAvailableShaders();
	auto foundShaderIt = std::find(availableShaders.begin(), availableShaders.end(), config.value("defaultShader", ""));
	shader.activate(std::distance(availableShaders.begin(), foundShaderIt));

	updateWindowTitle();

	ofAddListener(shader.onChange, this, &ofApp::onShaderChange);
}

//--------------------------------------------------------------
void ofApp::update() {
	for (auto entry : uniformValues) {
		auto value = uniformValues[entry.first];
		uniformValues[entry.first].current = lerp(value.current, value.target, 2 * ofGetLastFrameTime());
	}
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofEnableDepthTest();

	ndiFbo.begin();
	ofClear(0);
	shader.draw(renderSize);
	ofSetColor(13, 123, 231);
	ofDrawRectangle(100, 100, 200, 200);
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

void ofApp::windowResized(int w, int h) {
	float aspect = 16. / 9.;
	ofSetWindowShape(w, w / aspect);
}

void ofApp::keyPressed(ofKeyEventArgs& key)
{
	browseShaders(key);
	//selectShaderByNumber(key);
}

void ofApp::browseShaders(ofKeyEventArgs& key)
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
	shader.advance(increment);
}

void ofApp::selectShaderByNumber(ofKeyEventArgs& key) {
	if (key.key < 48 || key.key > 57) {
		return;
	}
	int index = 57 - key.key;
	shader.activate(index);
	//ofLogNotice("LiveCoder") << "key: " << key.key;
}

void ofApp::newMidiMessage(ofxMidiMessage& msg) {

	auto midiConfig = config.at("midi");
	if (midiConfig.value("debug", false)) {
		ofLogNotice("LiveCoder") << "onMidiMessage(): Input: " << msg.portName << ", pitch: " << msg.pitch << ", velocity: " << msg.velocity << ", control: " << msg.control << ", value: " << msg.value;
	}
	auto ccBindings = config.at("/midi/bindings/cc"_json_pointer);
	for (auto ccBinding : ccBindings.items()) {
		if (msg.control != ccBinding.value()) {
			continue;
		}
		uniformValues[ccBinding.key()].target = msg.value / 127.;
		//ofLogNotice("LiveCoder") << "onMidiMessage binding: " << ccBinding.key() << " to: " << ccBinding.value();
	}
	// Store CC value into a map
	// Uniform function takes value from cc
}

void ofApp::onShaderChange(pohy::ShaderInfo& info)
{
	pDropdownShader->select(info.index);
	updateWindowTitle();
}

void ofApp::setupGui() {
	pGui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	pLabelFps = pGui->addLabel("fps");
	pDropdownShader = pGui->addDropdown("shader", shader.getAvailableShaders());
	pDropdownShader->select(shader.getCurrentShaderInfo().index);
	pDropdownShader->onDropdownEvent([=](ofxDatGuiDropdownEvent e) {
		shader.activate(e.child);
		});
	pFolderUniforms = pGui->addFolder("Uniforms");
	pFolderUniforms->expand();
}

void ofApp::setupNdi() {
	ndiFbo.allocate(renderSize.x, renderSize.y, GL_RGBA);
	ndiSender.SetAsync();
	ndiSender.CreateSender(senderName.c_str(), renderSize.x, renderSize.y);
}

void ofApp::setupMidi() {
	ofxMidiIn midiIn;
	midiIn.listInPorts();
	auto requestedInputs = config.at("midi").value("inputs", std::vector<string>());
	for (auto input : midiIn.getInPortList()) {
		auto foundInput = std::find(requestedInputs.begin(), requestedInputs.end(), input);
		if (foundInput == requestedInputs.end()) {
			continue;
		}
		ofxMidiIn selectedInput;
		ofLogNotice("LiveCoder") << "Selecting input: " << *foundInput;
		selectedInput.openPort(*foundInput);
		selectedInput.addListener(this);
		midiIns.push_back(selectedInput);
	}
	auto ccBindings = config.at("/midi/bindings/cc"_json_pointer);
	for (auto ccBinding : ccBindings.items()) {
		auto uniformName = ccBinding.key();
		uniformValues[uniformName] = InterpolationValue({ 0,0 });
		shader.addUniformFunction(uniformName, UniformFunction([=](ofShader* _shader) {
			_shader->setUniform1f(uniformName, uniformValues[uniformName].current);
			}));
	}
}

void ofApp::updateWindowTitle()
{
	auto shaderName = shader.getCurrentShaderInfo().name;
	ofSetWindowTitle("Sender: " + senderName + " / Shader: " + shaderName);
}
