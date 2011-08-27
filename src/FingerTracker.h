#pragma once

#include "ofMain.h"
#include "ofSoundPlayer.h"
#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxSimpleGuiToo.h"
#include "DrumKit.h"
#include "ofColor.h"
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
    
    void unproject(unsigned short *depth, float *x, float *y, float *z);
    
    vector<cv::Point2i> detectFingers(cv::Mat1f z, float zMin=0.0f, float zMax=0.75f);

private:
    // Application + Kinect
	ofxKinect                       kinect;
    ofxCvGrayscaleImage             grayImage;
    ofxCvColorImage                 colorImage;    
	int                             angle;
    
    // Finger detection 
    std::vector<cv::Point2i>        fingerTips;
    cv::Mat                         *depthFrameRaw;
    cv::Mat                         *x;
    cv::Mat                         *y;
    cv::Mat                         *z;
    float                           rMin;
    float                           zMin;
    float                           zMax;
    int                             key;
    unsigned short                  *depthFrameRawData;
    
    // GUI 
    bool                            showMenu;
    bool                            showColorImage;
    
    // Drum 
    DrumKit                         drumKit;
    int                             framePlay;
    int                             drumCount;
    float                           minFreq;
    float                           maxFreq;
    
    
    ofColor setColor(int u, int v, int alpha=200);
};
