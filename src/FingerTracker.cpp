#include "FingerTracker.h"


//--------------------------------------------------------------
void FingerTracker::setup() {
    //Setup OF
    ofSetFrameRate(60);
    ofSetWindowTitle("FingerTracker");
    
	//Setup Kinect
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();
    angle = 0;
	kinect.setCameraTiltAngle(angle);
    
    //Setup Images
	grayImage.allocate(kinect.width, kinect.height);
    threshImage.allocate(kinect.width, kinect.height);
    
    //Setup detection
    nearThreshold = 255;
    farThreshold = 200;
    mirror = false;
    
    //Setup GUI
    gui.setup();
    gui.config->gridSize.x = 200;
    gui.addTitle("Kinect Settings");
    gui.addSlider("Tilt Angle", angle, -30, 30);
    gui.addToggle("Mirror", mirror);
    gui.addTitle("Depth Threshold");
    gui.addSlider("Near Distance", nearThreshold, 0, 255);
    gui.addSlider("Far Distance", farThreshold, 255, 0);
    gui.setDefaultKeys(true);
    gui.loadFromXML();
    gui.show();
}

//--------------------------------------------------------------
void FingerTracker::update() {
	ofBackground(55, 50, 50);
    kinect.setCameraTiltAngle(angle);

	kinect.update();
	if(kinect.isFrameNew())	{
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        threshImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        
        if(mirror) {
            grayImage.mirror(false,true);
            threshImage.mirror(false,true);
        }
        
        unsigned char * pix = threshImage.getPixels();
		int numPixels = threshImage.getWidth() * threshImage.getHeight();

		for(int i = 0; i < numPixels; i++){
            if( pix[i] < nearThreshold && pix[i] > farThreshold ){
                pix[i] = 255;
            } else {
                pix[i] = 0;
            }
        }
        
        //update the cv image
        grayImage.flagImageChanged();
        threshImage.flagImageChanged();
        contourFinder.findContours(threshImage, (kinect.width*kinect.height)/30, (kinect.width*kinect.height)/2, 20, false);
    }
}

//--------------------------------------------------------------
void FingerTracker::draw() {
    gui.draw();
	ofSetColor(255, 255, 255);
    
    kinect.drawDepth(400, 100, 400, 300);

    threshImage.draw(400, 420, 400, 300);
    contourFinder.draw(400, 420, 400, 300);

	ofSetColor(255, 255, 255);
}

//--------------------------------------------------------------
void FingerTracker::exit() {
	kinect.setCameraTiltAngle(0);
	kinect.close();
}

//--------------------------------------------------------------
void FingerTracker::keyPressed (int key) {
	switch (key) {
		case '>':
		case '.':
			farThreshold ++;
			if (farThreshold > 255) farThreshold = 255;
			break;
		case '<':		
		case ',':		
			farThreshold --;
			if (farThreshold < 0) farThreshold = 0;
			break;
			
		case '+':
		case '=':
			nearThreshold ++;
			if (nearThreshold > 255) nearThreshold = 255;
			break;
		case '-':		
			nearThreshold --;
			if (nearThreshold < 0) nearThreshold = 0;
			break;

		case OF_KEY_UP:
			angle++;
			if(angle>30) angle=30;
			kinect.setCameraTiltAngle(angle);
			break;

		case OF_KEY_DOWN:
			angle--;
			if(angle<-30) angle=-30;
			kinect.setCameraTiltAngle(angle);
			break;
	}
}

//--------------------------------------------------------------
void FingerTracker::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void FingerTracker::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void FingerTracker::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void FingerTracker::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void FingerTracker::windowResized(int w, int h) {}

