#include "FingerTracker.h"
#include "Constants.h"

//--------------------------------------------------------------
void FingerTracker::setup() {
    //Setup OF
    ofSetFrameRate(60);
	ofEnableAlphaBlending();
    ofSetWindowTitle("FingerTracker");
#ifdef __APPLE__
    ofSetDataPathRoot("../Resources/");
#endif
    
	//Setup Kinect
	kinect.init();
	kinect.setVerbose(true);
	kinect.open();
    angle = 0;
	kinect.setCameraTiltAngle(angle);
    
    //Setup Images
    grayImage.allocate(kinect.width, kinect.height);
    colorImage.allocate(kinect.width, kinect.height);
    depthFrameRaw =  new cv::Mat(480, 640, CV_16UC1);
    x = new cv::Mat(480, 640, CV_32FC1);
    y = new cv::Mat(480, 640, CV_32FC1);
    z = new cv::Mat(480, 640, CV_32FC1);
    rMin = 25;
    zMin = 0.0f;
    zMax = 0.75f;
#ifndef DEBUG
    showMenu = false;
    showImage = true;
#else
    showMenu = true;
    showImage = false;
#endif
    showColorImage = true;
    framePlay = drumKit.getFramePlay();
    drumCount = drumKit.getDrumCount();
    minFreq = drumKit.getMinFreq();
    maxFreq = drumKit.getMaxFreq();
    
    // Setup Fluid
    fluidSolver.setup(100, 100);
    fluidSolver.enableRGB(true).setFadeSpeed(FADESPEED).setDeltaT(DELTAT).setVisc(VISCOSITY).setColorDiffusion(COLORDIFF);
	fluidDrawer.setup( &fluidSolver );
	particleSystem.setFluidSolver( &fluidSolver );
	fluidCellsX			= 150;
    windowResized(ofGetWidth(), ofGetHeight());
	pMouse = getWindowCenter();
	resizeFluid			= true;
    moveFluid           = false;
    
    //Setup GUI
    gui.setup();
    gui.config->gridSize.x = 250;
    gui.addTitle("Kinect Settings");
    gui.addSlider("Tilt Angle", angle, -30, 30);
    gui.addSlider("Near Distance", zMin, 0.0f, 1.0f);
    gui.addSlider("Far Distance", zMax, 1.5f, 0.5f);
    gui.addTitle("Drum Controls");
    gui.addSlider("Frame Play", framePlay, 1, 30);
    gui.addSlider("Drum Count", drumCount, 10, 30);
    gui.addSlider("Min Frequency", minFreq, 0.1f, 1.0f);
    gui.addSlider("Max Frequency", maxFreq, 1.0f, 20.0f);
    gui.addPage();
    gui.addTitle("Fluid Settings");
    gui.addSlider("fluidCellsX", fluidCellsX, 20, 400);
	gui.addButton("resizeFluid", resizeFluid);
	gui.addSlider("fs.viscocity", fluidSolver.viscocity, 0.0, 0.01); 
	gui.addSlider("fs.colorDiffusion", fluidSolver.colorDiffusion, 0.0, 0.0003); 
	gui.addSlider("fs.fadeSpeed", fluidSolver.fadeSpeed, 0.0, 0.1); 
	gui.addSlider("fs.solverIterations", fluidSolver.solverIterations, 1, 50); 
	gui.addSlider("fs.deltaT", fluidSolver.deltaT, 0.1, 5);
	gui.addComboBox("fd.drawMode", (int&)fluidDrawer.drawMode, kFluidDrawCount, (string*)FluidDrawerGl::drawOptionTitles);
	gui.addToggle("fs.doRGB", fluidSolver.doRGB); 
	gui.addToggle("fs.doVorticityConfinement", fluidSolver.doVorticityConfinement);
	gui.addToggle("fs.wrapX", fluidSolver.wrap_x); 
	gui.addToggle("fs.wrapY", fluidSolver.wrap_y);
    gui.setDefaultKeys(true);
    //gui.loadFromXML();
    gui.setPage(1);
    gui.show();
}

//--------------------------------------------------------------
void FingerTracker::update() {
	ofBackground(60, 50, 50);
    kinect.setCameraTiltAngle(angle);
	kinect.update();
    
    if(resizeFluid) {
		fluidSolver.setSize(fluidCellsX, fluidCellsX / getWindowAspectRatio() );
		fluidDrawer.setup(&fluidSolver);
		resizeFluid = false;
	}
    
	if(kinect.isFrameNew())	{
        drumKit.setFramePlay(framePlay);
        drumKit.setDrumCount(drumCount);
        drumKit.setMinFreq(minFreq);
        drumKit.setMaxFreq(maxFreq);
        
        colorImage.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        depthFrameRawData = kinect.getRawDepthPixels();
        
        unproject(depthFrameRawData, (float*)x->data, (float*)y->data, (float*)z->data);
        std::vector<cv::Point2i> oldFingers = fingerTips;
        fingerTips = detectFingers(*z, zMin, zMax);
        float vx, vy;
        #pragma omp parallel for
        for(int i=0;i<fingerTips.size();i++) {
            vx = i<oldFingers.size() ? fingerTips[i].x - oldFingers[i].x : 0.0f;
            vy = i<oldFingers.size() ? fingerTips[i].y - oldFingers[i].y : 0.0f;
            if(vx==0 && vy==0) {
                vx = ofRandom(-0.01, 0.01);
                vy = ofRandom(-0.01, 0.01);
            }
            addToFluid( MSA::Vec2f(
                                   fingerTips[i].x*(float)ofGetWidth()/kinect.width,
                                   fingerTips[i].y*(float)ofGetHeight()/kinect.height
                        )/getWindowSize(),
                        MSA::Vec2f(vx, vy)/getWindowSize(),
                        !moveFluid, true );
        }
        drumKit.play(fingerTips, kinect.width, kinect.height);
    }
    
	fluidSolver.update();
}

//--------------------------------------------------------------
void FingerTracker::draw() {
    ofSetColor(255, 255, 255);
    
    // Draw Fluid
	glColor3f(1, 1, 1);
    fluidDrawer.draw(0, 0, getWindowWidth(), getWindowHeight());
    particleSystem.updateAndDraw( false );
    
    // Draw Pictures
    ofEnableAlphaBlending();
	if(!showImage) {
        ofSetColor(255, 255, 255, 20);
        grayImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    }
    
	ofSetColor(255, 255, 255);
    drumKit.draw();
    
    // Show control
    if(showColorImage) {
        ofSetColor(255, 255, 255);
        colorImage.draw(ofGetWidth()-160, ofGetHeight()-120, 160, 120);
    }
    if(showMenu)
        gui.draw();
}

//--------------------------------------------------------------
void FingerTracker::exit() {
	kinect.setCameraTiltAngle(0);
	kinect.close();
    delete depthFrameRaw;
    delete x;
    delete y;
    delete z;
}

//--------------------------------------------------------------
void FingerTracker::keyPressed (int key) {
	switch (key) {
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
        
        case 'i':
        case 'I':
            showImage = !showImage;
            break;
            
        case 's':
        case 'S':
            showMenu = !showMenu;
            break;
            
        case 'c':
        case 'C':
            showColorImage = !showColorImage;
            break;
            
        case 'e':
        case 'E':
            moveFluid = !moveFluid;
            break;
            
        case 'r':
        case 'R':
            fluidSolver.reset();
            break;
            
        case 'm':
        case 'M':
            drumKit.toggleMute();
            break;
        
        case 'f':
        case 'F': 
            ofToggleFullscreen();
            break;
	}
}

//--------------------------------------------------------------
void FingerTracker::mouseMoved(int x, int y) {
	Vec2f eventPos = Vec2f(x, y);
	Vec2f mouseNorm = Vec2f( eventPos) / getWindowSize();
	Vec2f mouseVel = Vec2f( eventPos - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, true, true );
	pMouse = eventPos;
}

//--------------------------------------------------------------
void FingerTracker::mouseDragged(int x, int y, int button)  {
	Vec2f eventPos = Vec2f(x, y);
	Vec2f mouseNorm = Vec2f( eventPos ) / getWindowSize();
	Vec2f mouseVel = Vec2f( eventPos - pMouse ) / getWindowSize();
	addToFluid( mouseNorm, mouseVel, false, true );
	pMouse = eventPos;
}

//--------------------------------------------------------------
void FingerTracker::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void FingerTracker::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void FingerTracker::windowResized(int w, int h) {    
	particleSystem.setWindowSize( Vec2f( w, h ) );
}
