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
	nebulaFbo.allocate(w, h, GL_RGBA);

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

	nebulaShaderPath = "shaders/nebula";
	nebulaShader.load(nebulaShaderPath);

	//*********************************
	//	Particles
	//*********************************
	particles.setup(sp_x_dim, sp_y_dim);
	particles.color1 = ofColor(239, 119, 66);
	particles.color2 = ofColor(20, 12, 44);
	particles.color3 = ofColor(227, 204, 163);
	particles.color4 = ofColor(147, 61, 60);
	particles.color5 = ofColor(41, 16, 44);
	particles.initColorTexture();

	//*********************************
	//	Spout
	//*********************************
	spout.init("paint");
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

	//	Pass data to particles
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
	//	Draw Textures
	//*********************************
	nebulaFbo.begin();
	nebulaShader.begin();
	nebulaShader.setUniform1f("iTime", t);
	nebulaShader.setUniform2f("iMouse", 0, 0);
	nebulaShader.setUniform2f("iResolution", w, h);
	drawScreenPlane();
	nebulaShader.end();
	nebulaFbo.end();

	//*********************************
	//	Draw Interactions
	//*********************************
	drawFbo.begin();
	drawFade();

	particles.draw();

	revealShader.begin();
	revealShader.setUniform1f("radius", 0.01 + 0.19 * ofNoise(1.0 * t));
	revealShader.setUniformTexture("image", nebulaFbo.getTexture(), 0);
	revealShader.setUniform2fv("lHands", &lHands[0].x, 4);
	revealShader.setUniform2fv("rHands", &rHands[0].x, 4);
	revealShader.setUniform2f("offset", sampleOffset);
	revealShader.setUniform2f("lOffset", lHands[0] - sampleOffset);
	revealShader.setUniform2f("rOffset", rHands[0] - sampleOffset);
	revealShader.setUniform2f("res", w, h);
	drawScreenPlane();
	revealShader.end();

	ofPushStyle();
	ofSetColor(255, 0, 0);
	ofDrawCircle(0.5 * w, 0.5 * h, 100);
	ofPopStyle();

	drawFbo.end();
	drawFbo.draw(0, 0);

	//*********************************
	//	Interaction Test
	//*********************************
	//drawHands();

	//*********************************
	//	Spout
	//*********************************
	spout.send(drawFbo.getTexture());
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
	ofSetColor(ofFloatColor(0, 0, 0, 0.005));
	ofDrawRectangle(0, 0, w, h);
	ofPopStyle();
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

	switch (key)
	{
	case 'r':
		//	Reload shaders
		revealShader.unload();
		revealShader.load(revealShaderPath);

		nebulaShader.unload();
		nebulaShader.load(nebulaShaderPath);

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