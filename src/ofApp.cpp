#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){

	//*********************************
	//  OF Settings
	//*********************************
	ofSetFrameRate(60);
	//ofSetFullscreen(true);

	//*********************************
	//  Data
	//*********************************
	w = ofGetWidth();
	h = ofGetHeight();

	//*********************************
	//  Behavior
	//*********************************
	sampleOffset = glm::vec2(w * 0.5, h * 0.5);

	//*********************************
	//  FBOs
	//*********************************
	drawFbo.allocate(w, h, GL_RGBA32F_ARB);

	//*********************************
	//	Kinect
	//*********************************
	kinect.open();
	kinect.initBodySource();
	k_xscale = w / KD_WIDTH;
	k_yscale = h / KD_HEIGHT;
	k_xoffset = 0.0f;
	k_yoffset = 0.0f;

	//*********************************
	//	Images
	//*********************************
	img.load("images/nebula.png");

	//*********************************
	//	Plane
	//*********************************
	plane.set(w, h);
	plane.mapTexCoords(0, 0, w, h);

	//*********************************
	//	Shaders
	//*********************************
	revealShaderPath = "shaders/reveal";
	revealShader.load(revealShaderPath);
}

//--------------------------------------------------------------
void ofApp::update(){

	//*********************************
	//  Data
	//*********************************
	t = ofGetElapsedTimef();
	f = ofGetFrameNum();

	//*********************************
	//	Interaction
	//*********************************
	kinect.update();
	auto bodies = kinect.getBodySource()->getBodies();

	int i = 0;
	for (auto body : bodies)
	{
		if (body.joints.size() < 20)
		{
			continue;
		}

		auto leftHand = body.joints.at(JointType_HandLeft);
		auto rightHand = body.joints.at(JointType_HandRight);

		lHands[i] = glm::vec2(leftHand.getPositionInDepthMap().x * k_xscale + k_xoffset, leftHand.getPositionInDepthMap().y * k_yscale + k_yoffset);
		rHands[i] = glm::vec2(rightHand.getPositionInDepthMap().x * k_xscale + k_xoffset, rightHand.getPositionInDepthMap().y * k_yscale + k_yoffset);

		++i;
		if (i == 4) break;
	}

	if (f % 60 >= 59) {
		sampleOffset.x = ofRandom(0, w);
		sampleOffset.y = ofRandom(0, h);
	}

	//*********************************
	//	Window
	//*********************************
	ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){

	//*********************************
	//	Background
	//*********************************
	ofBackground(0);

	//*********************************
	//	Reveal Layer
	//*********************************
	drawFbo.begin();
	drawFade();
	revealShader.begin();
	revealShader.setUniform1f("radius", 0.01 + 0.05 * ofNoise(0.5 * t));
	revealShader.setUniformTexture("image", img.getTexture(), 0);
	revealShader.setUniform2fv("lHands", &lHands[0].x, 4);
	revealShader.setUniform2fv("rHands", &rHands[0].x, 4);
	revealShader.setUniform2f("lOffset", lHands[0] - sampleOffset);
	revealShader.setUniform2f("rOffset", rHands[0] - sampleOffset);
	revealShader.setUniform2f("res", w, h);
	drawScreenPlane();
	revealShader.end();
	drawFbo.end();
	drawFbo.draw(0, 0);

	//*********************************
	//	Interaction Test
	//*********************************
	//drawHands();
}

//--------------------------------------------------------------
void ofApp::drawHands() {

	ofPushStyle();
	ofSetColor(255);
	for (auto pos : lHands)
	{
		ofDrawCircle(pos.x, pos.y, 10);
	}
	for (auto pos : rHands)
	{
		ofDrawCircle(pos.x, pos.y, 10);
	}
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::drawScreenPlane() {

	ofPushMatrix();
	ofTranslate(0.5 * w, 0.5 * h);
	plane.draw();
	ofPopMatrix();
}

//--------------------------------------------------------------
void ofApp::drawFade() {

	ofPushStyle();
	ofSetColor(0, 0, 0, 4);
	ofDrawRectangle(0, 0, w, h);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key)
	{
	case 'r':
		revealShader.unload();
		revealShader.load(revealShaderPath);
		break;
	default:
		break;
	}
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y){

}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
