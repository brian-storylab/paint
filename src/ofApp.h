#pragma once

#include "ofMain.h"

//	Addons
#include "ofxKinectForWindows2.h"
#include "ofxSpout.h"

//	Project Headers
#include "SP_Particles.h"

//	Definitions
#define KD_WIDTH 512.0
#define KD_HEIGHT 424.0

class ofApp : public ofBaseApp{

public:

	void setup();
	void update();
	void draw();

	void drawHands();
	void drawScreenPlane();
	void drawFade();

	void keyPressed(int key);
	void keyReleased(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void mouseEntered(int x, int y);
	void mouseExited(int x, int y);
	void windowResized(int w, int h);
	void dragEvent(ofDragInfo dragInfo);
	void gotMessage(ofMessage msg);

private:

	//*********************************
	//	Data
	//*********************************
	float w, h, t;
	int f;

	//*********************************
	//	Behavior
	//*********************************
	glm::vec2 sampleOffset;

	//*********************************
	//	FBOs
	//*********************************
	ofFbo drawFbo;
	ofFbo nebulaFbo;

	//*********************************
	//	Kinect
	//*********************************
	ofxKFW2::Device kinect;
	glm::vec2 lHands[4], rHands[4];
	float k_xscale, k_yscale;
	float k_xoffset, k_yoffset;

	//*********************************
	//	Images
	//*********************************
	ofImage img;

	//*********************************
	//	Plane
	//*********************************
	ofPlanePrimitive plane;

	//*********************************
	//	Shaders
	//*********************************
	ofShader revealShader;
	string revealShaderPath;

	ofShader nebulaShader;
	string nebulaShaderPath;

	//*********************************
	//	Particles
	//*********************************
	SP_Particles particles;
	const int sp_x_dim = 75;
	const int sp_y_dim = 75;

	//*********************************
	//	Spout
	//*********************************
	ofxSpout::Sender spout;
};
