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

	//*********************************
	//	Particles
	//*********************************
	particles.setup(sp_x_dim, sp_y_dim);
	particles.r = 4.0;
	particles.color1 = ofColor(54, 152, 163);
	particles.color2 = ofColor(140, 68, 53);
	particles.color3 = ofColor(60, 48, 91);
	particles.initColorTexture();
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

	for (int i = 0; i < 4; ++i)
	{
		lHands[i] = glm::vec2(-1.0, -1.0);
		rHands[i] = glm::vec2(-1.0, -1.0);
	}

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

	if (f % 120 >= 119) {
		sampleOffset.x = ofRandom(0.15 * w, 0.85 * w);
		sampleOffset.y = ofRandom(0.15 * h, 0.85 * h);
	}

	for (int i = 0; i < 4; ++i)
	{
		particles.lHands[i] = lHands[i];
		particles.rHands[i] = rHands[i];
	}
	
	particles.update();

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
	//	Draw Main
	//*********************************
	drawFbo.begin();
	drawFade();

	particles.draw();

	revealShader.begin();
	revealShader.setUniform1f("radius", 0.01 + 0.14 * ofNoise(1.0 * t));
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
	ofSetColor(0, 0, 0, 1);
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
		particles.reloadShaders();
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
