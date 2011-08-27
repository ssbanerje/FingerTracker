#pragma once

#include "ofMain.h"
#include "ofSoundPlayer.h"
#include "ofColor.h"

#include "ofxOpenCv.h"
#include "ofxKinect.h"
#include "ofxSimpleGuiToo.h"
#include "MSAFluid.h"
#include "MSATimer.h"


#include "DrumKit.h"
#include "ParticleSystem.h"

#include <vector>

using namespace std;

class FingerTracker: public ofBaseApp {
public:
    // openFrameworks
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
    
    // FingerDetection
    void unproject(unsigned short *depth, float *x, float *y, float *z);
    vector<cv::Point2i> detectFingers(cv::Mat1f z, float zMin=0.0f, float zMax=0.75f);
    
    // Fluid
    void fadeToColor(float r, float g, float b, float speed);
	void addToFluid(MSA::Vec2f pos, MSA::Vec2f vel, bool addColor, bool addForce);
    
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
    ofColor setColor(int u, int v, int alpha=200);
    
    // Drum 
    DrumKit                         drumKit;
    int                             framePlay;
    int                             drumCount;
    float                           minFreq;
    float                           maxFreq;
    
    // Fluid
	int					fluidCellsX;
	bool				resizeFluid;
	bool				drawFluid;
	bool				drawParticles;
	MSA::FluidSolver	fluidSolver;
	MSA::FluidDrawerGl	fluidDrawer;	
	ParticleSystem		particleSystem;
	MSA::Vec2f			pMouse;

    
};
