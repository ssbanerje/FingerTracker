#pragma once

#include "ofMain.h"
#include "ofSoundPlayer.h"
#include "cv.h"
#include "Constants.h"

using namespace std;

class DrumKit {
public:
    DrumKit();
    ~DrumKit() {}
    
    void draw();
    void play(vector<cv::Point2i> fingerTips);
    void setFramePlay(int val) {framePlay = val;}
    int  getFramePlay() {return framePlay;}    
    void setDrumCount(int val) {drumCount = val;}
    int  getDrumCount() {return drumCount;}
    
private:
    ofSoundPlayer           drumSounds[10];
    int                     framePlay;
    int                     drumCount;
};