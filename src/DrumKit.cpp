#include "DrumKit.h"

//--------------------------------------------------------------
DrumKit::DrumKit() {
    //Setup sounds
#ifdef __APPLE__
    ofSetDataPathRoot("../Resources/");
#endif
    #pragma omp parallel for
    for(int i=0;i<10;i++) {
        stringstream filename;
        filename << "kick_0"<<i+1<<".wav";
        drumSounds[i].loadSound(filename.str());
        drumSounds[i].setVolume(1.0f);
        drumSounds[i].setMultiPlay(true);
    }
    
    framePlay = FRAMEPLAY;
    drumCount = DRUMS;
}

//--------------------------------------------------------------
void DrumKit::draw() {
    ofSetColor(0,0,255, 50);
    #pragma omp parallel for
    for(int i=0;i<drumCount;i++)
        ofLine(i*ofGetWidth()/drumCount, 0, i*ofGetWidth()/drumCount, ofGetHeight());
}

//--------------------------------------------------------------
void DrumKit::play(vector<cv::Point2i> fingerTips, int kWidth, int kHeight) {
    if(ofGetFrameNum()%framePlay==0) {
        #pragma omp parallel for
        for(vector<cv::Point2i>::iterator it=fingerTips.begin(); it!=fingerTips.end(); it++) {
            drumSounds[(it->x/(kWidth/drumCount))%10].setSpeed(9.5*(float)it->y/kHeight + 0.5);
            drumSounds[(it->x/(kWidth/drumCount))%10].play();
        }
    }
}