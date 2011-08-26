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
    ofSetColor(255,0,0, 50);
    #pragma omp parallel for
    for(int i=0;i<drumCount;i++)
        ofLine(i*WIDTH/drumCount, 0, i*WIDTH/drumCount, HEIGHT);
}

//--------------------------------------------------------------
void DrumKit::play(vector<cv::Point2i> fingerTips) {
    if(ofGetFrameNum()%framePlay==0) {
        #pragma omp parallel for
        for(vector<cv::Point2i>::iterator it=fingerTips.begin(); it!=fingerTips.end(); it++) {
            drumSounds[(it->x/(WIDTH/drumCount))%10].play();        }
    }
}