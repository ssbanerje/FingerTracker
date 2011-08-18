#pragma once

#include "ofMain.h"
#include "ofxPoint2f.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxSimpleGuiToo.h"
#include <vector>

using namespace std;

class FingerTracker: public ofBaseApp {
public:
    void setup();
	void update();
	void draw();
	void exit();
	void keyPressed(int key);
	void mouseMoved(int x, int y );
	void mouseDragged(int x, int y, int button);
	void mousePressed(int x, int y, int button);
	void mouseReleased(int x, int y, int button);
	void windowResized(int w, int h);
    
    //vector<ofxPoint2f> detectFingers(cv::Mat1f z, float zMin=0.0f, float zMax=0.75f, cv::Mat1f& debugFrame=cv::Mat1f());

private:
	ofxKinect kinect;

	ofxCvGrayscaleImage 	grayImage;
    ofxCvGrayscaleImage     threshImage;

	ofxCvContourFinder 	contourFinder;

	int 				nearThreshold;
	int					farThreshold;
	int					angle;
    bool                mirror;
};
