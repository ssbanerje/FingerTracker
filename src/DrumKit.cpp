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
}

//--------------------------------------------------------------
void DrumKit::draw() {
    ofSetColor(255,0,0, 100);
    #pragma omp parallel for
    for(int i=0;i<DRUMS;i++)
        ofLine(i*WIDTH/DRUMS, 0, i*WIDTH/DRUMS, HEIGHT);
}

//--------------------------------------------------------------
void DrumKit::play(vector<cv::Point2i> fingerTips) {
    if(ofGetFrameNum()%FRAMEPLAY==0) {
        for(vector<cv::Point2i>::iterator it=fingerTips.begin(); it!=fingerTips.end(); it++) {
            drumSounds[(it->x/(WIDTH/DRUMS))%10].play();        }
    }
}