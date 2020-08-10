#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup() {
	config = json::parse(ofBufferFromFile("config.json", false).getText());

	ofEnableDepthTest();
	ofDisableArbTex();
	ofSetVerticalSync(true);
	ofSetFrameRate(config.value("fps", 60));

	auto senderName = config.at("/ndi"_json_pointer).value("senderName", "GLSL Live coder");

	ofSetWindowTitle("Sender: " + senderName);

	pGui = new ofxDatGui(ofxDatGuiAnchor::TOP_RIGHT);
	pButton = pGui->addButton("Clicker");

	ndiFbo.allocate(ofGetWidth(), ofGetHeight(), GL_RGBA);
	ndiSender.SetReadback();
	ndiSender.SetAsync();
	ndiSender.CreateSender(senderName.c_str(), ofGetWidth(), ofGetHeight());

	shader.load("shader");

	box.set(400);
	box.setPosition(ofGetWidth() / 2, ofGetHeight() / 2, 0);
}

//--------------------------------------------------------------
void ofApp::update() {
	pGui->update();
	box.rotateDeg(.3, { 0, 1, 0 });
	box.rotateDeg(.2, { 1, 0, 1 });
}

//--------------------------------------------------------------
void ofApp::draw() {
	ofEnableDepthTest();
	ofBackground(5);
	ofSetColor(255);

	ndiFbo.begin();
	ofClear(0, 0, 0, 0);
	shader.begin();
	float time = pButton->getMouseDown() ? 0 : ofGetElapsedTimef();
	shader.setUniform1f("iTime", time);
	shader.setUniform2f("iResolution", ofGetWidth(), ofGetHeight());
	ofDrawRectangle(0, 0, ofGetWidth(), ofGetHeight());
	shader.end();

	ndiFbo.end();

	ndiFbo.draw(0, 0);

	ndiSender.SendImage(ndiFbo);

	ofDisableDepthTest();
	pGui->draw();
}

void ofApp::exit() {
	ndiSender.ReleaseSender();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key) {

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key) {

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button) {

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y) {

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h) {

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg) {

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo) {

}
