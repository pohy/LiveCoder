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

	ofAddListener(shader.onChange, this, &ofApp::onShaderChange);

	senderName = config.at("/ndi"_json_pointer).value("senderName", "GLSL Live coder");

	auto configShaders = config.at("/shaders"_json_pointer);
	auto shaderDirectory = configShaders.value("directory", ".");
	auto importDirectory = configShaders.value("importDirectory", ".");
	downloadShaders(importDirectory);
	shader.load({ shaderDirectory, importDirectory });
	shader.activate(configShaders.value("default", ""));

	setupGui();
	setupNdi();
	setupMidi();

	updateWindowTitle();
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

void ofApp::onShaderChange(pohy::ShaderInfo& info) {
	if (pDropdownShader != nullptr) {
		pDropdownShader->select(info.index);
	}
	updateWindowTitle();

	// TODO: Update shader textures
	auto shaderTextureInfos = config.value("textures", std::map<string, std::map<string, string>>());
	if (!shaderTextureInfos.count(info.name)) {
		return;
	}
	auto shaderUniformTextures = shaderTextureInfos[info.name];
	for (auto uniformTexture : shaderUniformTextures) {
		// TODO: Shader does not track reference of shaderName -> (uniform, texture)
		//		 We should cache the loaded textures in memory, instead of loading from the file system every time
		//		 We could pass the whole structure to the shader
		//		 We could also keep track of the reference in ofApp
		ofImage textureImage;
		if (!ofFile::doesFileExist(uniformTexture.second)) {
			ofLogError("LiveCoder") << "onShaderChange(): Texture file '" << uniformTexture.first << "' does not exist";
			continue;
		}
		ofLoadImage(textureImage, uniformTexture.second);
		// TODO: Rotate the texture only for Shadertoy imports?
		textureImage.rotate90(2);
		ofTexture texture = textureImage.getTextureReference();
		texture.setTextureWrap(GL_REPEAT, GL_REPEAT);
		shader.setUniformTexture(uniformTexture.first, texture);
	}
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

void ofApp::downloadShaders(string downloadDirectory) {
	auto remoteShadersUnfiltered = config.value("remoteShaders", std::vector<string>());
	std::vector<string> remoteShaders(remoteShadersUnfiltered.size());
	std::copy_if(remoteShadersUnfiltered.begin(), remoteShadersUnfiltered.end(), remoteShaders.begin(), [](string url) {
		return url.find("https://www.shadertoy.com/view/") != string::npos;
		});
	for (auto remoteShader : remoteShaders) {
		auto lastSlashIndex = remoteShader.rfind("/");
		auto shaderId = remoteShader.substr(lastSlashIndex);
		stringstream  shaderJsonUrl;
		shaderJsonUrl << "https://www.shadertoy.com/api/v1/shaders/";
		shaderJsonUrl << shaderId;
		shaderJsonUrl << "?key=rtHKMM";
		// TODO: How about async?
		auto result = ofLoadURL(shaderJsonUrl.str());
		if (result.status >= 300) {
			ofLogError("LiveCoder") << "downloadShaders(): Shader loading failed: URL: " << shaderJsonUrl.str() << ", Error: " << result.error << ", Data:\n" << result.data.getText();
			continue;
		}
		auto shaderJson = json::parse(result.data.getText());
		try {
			auto shaderInfoJson = shaderJson.at("/Shader/info"_json_pointer);
			auto shaderName = shaderInfoJson.value("name", shaderId);
			auto shaderPath = ofFilePath::join(downloadDirectory, "_fromShaderToy_" + shaderName + ".frag");
			auto shaderExists = ofFile::doesFileExist(shaderPath);
			if (shaderExists) {
				ofLogNotice("LiveCoder") << "downloadShaders(): Shader at path " << shaderPath << " already exists. Skipping write";
				continue;
			}
			auto shaderPasses = shaderJson.at("/Shader/renderpass"_json_pointer);
			ofBuffer shaderSource;
			shaderSource = shaderPasses[0].value("code", "");
			auto writeResult = ofBufferToFile(shaderPath, shaderSource, false);
			if (!writeResult) {
				ofLogError("LiveCoder") << "downloadShaders(): Failed to write shader source to: " << shaderPath;
				continue;
			}
			ofLogNotice("LiveCoder") << "downloadShaders(): Shader written: " << shaderPath;
		}
		catch (json::out_of_range ex) {
			//ofLogError("LiveCoder") << "downloadShaders(): Could not load shader info. URL: " << remoteShader;
			ofLogError("LiveCoder") << "downloadShaders(): json out_of_range: " << ex.what() << ", URL: " << remoteShader << " Shader JSON: \n" << shaderJson.dump();
			continue;
		}
	}
}

void ofApp::updateWindowTitle()
{
	auto shaderName = shader.getCurrentShaderInfo().name;
	ofSetWindowTitle("Sender: " + senderName + " / Shader: " + shaderName);
}
