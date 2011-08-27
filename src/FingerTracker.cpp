#include "FingerTracker.h"
#include "Constants.h"


//--------------------------------------------------------------
void FingerTracker::setup() {
    //Setup OF
    ofSetFrameRate(60);
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
#else
    showMenu = true;
#endif
    showColorImage = true;
    framePlay = drumKit.getFramePlay();
    drumCount = drumKit.getDrumCount();
    minFreq = drumKit.getMinFreq();
    maxFreq = drumKit.getMaxFreq();
    
    //Setup GUI
    gui.setup();
    gui.config->gridSize.x = 200;
    gui.addTitle("Kinect Settings");
    gui.addSlider("Tilt Angle", angle, -30, 30);
    gui.addSlider("Near Distance", zMin, 0.0f, 1.0f);
    gui.addSlider("Far Distance", zMax, 1.5f, 0.5f);
    gui.addTitle("Drum Controls");
    gui.addSlider("Frame Play", framePlay, 1, 30);
    gui.addSlider("Drum Count", drumCount, 10, 30);
    gui.addSlider("Min Frequency", minFreq, 0.1f, 1.0f);
    gui.addSlider("Max Frequency", maxFreq, 1.0f, 20.0f);
    gui.setDefaultKeys(true);
    //gui.loadFromXML();
    gui.show();
}

//--------------------------------------------------------------
void FingerTracker::update() {
	ofBackground(60, 50, 50);
    kinect.setCameraTiltAngle(angle);
	kinect.update();
    
	if(kinect.isFrameNew())	{
        drumKit.setFramePlay(framePlay);
        drumKit.setDrumCount(drumCount);
        drumKit.setMinFreq(minFreq);
        drumKit.setMaxFreq(maxFreq);
        
        colorImage.setFromPixels(kinect.getPixels(), kinect.width, kinect.height);
        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
        depthFrameRawData = kinect.getRawDepthPixels();
        
        unproject(depthFrameRawData, (float*)x->data, (float*)y->data, (float*)z->data);
        fingerTips = detectFingers(*z, zMin, zMax);
        drumKit.play(fingerTips, kinect.width, kinect.height);
    }
}

//--------------------------------------------------------------
void FingerTracker::unproject(unsigned short *depth, float *x, float *y, float *z) {
    int u,v;
	const float f = 500.0f;
	const float u0 = 320.0f;
	const float v0 = 240.0f;
	float zCurrent;
    
    #pragma omp parallel for
	for (int i=0; i<640*480; i++) {
		u = i % 640;
		v = i / 640;
		zCurrent = 1.0f / (-0.00307110156374373f * depth[i] + 3.33094951605675f);
		if (z != NULL) {
			z[i] = zCurrent;
		}
		if (x != NULL) {
			x[i] = (u - u0) * zCurrent / f;
		}
		if (y != NULL) {
			y[i] = (v - v0) * zCurrent / f;
		}
	}
}

//--------------------------------------------------------------
vector<cv::Point2i> FingerTracker::detectFingers(cv::Mat1f z, float zMin, float zMax) {
    vector<cv::Point2i> fingers;
    
    cv::Mat handMask = z < zMax & z > zMin;
    std::vector< std::vector<cv::Point> > contours;
    
    cv::findContours(handMask, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
    if(contours.size()) {
        for (int i=0; i<contours.size(); i++) {
            vector<cv::Point> contour = contours[i];
            cv::Mat contourMat(contour);
            double area = cv::contourArea(contourMat);
            
            if(area > 3000) {
                cv::Scalar center = cv::mean(contourMat);
                cv::Point centerPoint(center.val[0], center.val[1]);
                vector<cv::Point> approxCurve;
                cv::approxPolyDP(contourMat, approxCurve, 20, true);
                vector<int> hull;
                cv::Mat temp(approxCurve);
                cv::convexHull(temp, hull);
                
                int upper=640, lower=0;
                #pragma omp parallel for
                for(int j=0;j<hull.size();j++) {
                    int idx = hull[j];
					if (approxCurve[idx].y < upper) upper = approxCurve[idx].y;
					if (approxCurve[idx].y > lower) lower = approxCurve[idx].y;
                }
                
                float cutoff = lower - (lower-upper)*0.1f;
            
                #pragma omp parallel for
                for (int j=0; j<hull.size(); j++) {
					int idx = hull[j];
					int pdx = idx == 0 ? approxCurve.size() - 1 : idx - 1;
					int sdx = idx == approxCurve.size() - 1 ? 0 : idx + 1;
                    
                    cv::Point v1 = approxCurve[sdx] - approxCurve[idx];
                    cv::Point v2 = approxCurve[pdx] - approxCurve[idx];
                    
					float angle = acos( (v1.x*v2.x + v1.y*v2.y) / (norm(v1) * norm(v2)) );
                    
					if (angle < 1 && approxCurve[idx].y < cutoff) {
						int u = approxCurve[idx].x;
						int v = approxCurve[idx].y;
            
						fingers.push_back(cv::Point2i(u,v));
					}
				}
            }
        }
    }
    return fingers;
}

//--------------------------------------------------------------
void FingerTracker::draw() {
    ofEnableAlphaBlending();
	ofSetColor(100, 100, 100);
    grayImage.draw(0, 0, ofGetWidth(), ofGetHeight());
    for(vector<cv::Point2i>::iterator it=fingerTips.begin(); it!=fingerTips.end(); it++) {
        ofSetColor(setColor(it->x, it->y));
        ofCircle(it->x*ofGetWidth()/kinect.width, it->y*ofGetHeight()/kinect.height, 10);
    }
	ofSetColor(255, 255, 255);
    drumKit.draw();
    ofSetColor(255, 255, 255);
    if(showColorImage)
        colorImage.draw(ofGetWidth()-160, ofGetHeight()-120, 160, 120);
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
        
        case 's':
        case 'S':
            showMenu = !showMenu;
            break;
            
        case 'c':
        case 'C':
            showColorImage = !showColorImage;
            break;
	}
}

//--------------------------------------------------------------
inline ofColor FingerTracker::setColor(int u, int v, int alpha) {
    float h = ((float)u/kinect.width);
    float s = 1.0f;
    float l = 0.5 + 0.25*(float)v/kinect.height;
    unsigned int r, g, b;
    
	if(s == 0) {
		r = l;
		g = l;
		b = l;
	} else {
		double temp1 = 0;
		if(l < .50) {
			temp1 = l*(1 + s);
		} else {
			temp1 = l + s - (l*s);
		}
        
		double temp2 = 2*l - temp1;
        
		double temp3 = 0;
        #pragma omp parallel for
		for(int i = 0 ; i < 3 ; i++) {
			switch(i) {
                case 0: // red
				{
					temp3 = h + .33333;
					if(temp3 > 1)
						temp3 -= 1;
					HSLtoRGB_Subfunction(r,temp1,temp2,temp3);
					break;
				}
                case 1: // green
				{
					temp3 = h;
					HSLtoRGB_Subfunction(g,temp1,temp2,temp3);
					break;
				}
                case 2: // blue
				{
					temp3 = h - .33333;
					if(temp3 < 0)
						temp3 += 1;
					HSLtoRGB_Subfunction(b,temp1,temp2,temp3);
					break;
				}
                default:
				{
                    
				}
			}
		}
	}
	r = (uint)((((double)r)/100)*255);
	g = (uint)((((double)g)/100)*255);
	b = (uint)((((double)b)/100)*255);
    return ofColor(r, g, b, alpha);
}

//--------------------------------------------------------------
void FingerTracker::HSLtoRGB_Subfunction(uint& c, const double& temp1, const double& temp2, const double& temp3) {
    if((temp3 * 6) < 1)
        c = (uint)((temp2 + (temp1 - temp2)*6*temp3)*100);
    else
        if((temp3 * 2) < 1)
            c = (uint)(temp1*100);
        else
            if((temp3 * 3) < 2)
                c = (uint)((temp2 + (temp1 - temp2)*(.66666 - temp3)*6)*100);
            else
                c = (uint)(temp2*100);
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

