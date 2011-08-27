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
    void play(vector<cv::Point2i> fingerTips, int kWidth, int kHeight);
    
    void setFramePlay(int val) {framePlay = val;}
    int  getFramePlay() {return framePlay;}    
    
    void setDrumCount(int val) {drumCount = val;}
    int  getDrumCount() {return drumCount;}
    
    void setMinFreq(float val) {minFreq = val;}
    float  getMinFreq() {return minFreq;}
    
    void setMaxFreq(float val) {maxFreq = val;}
    float  getMaxFreq() {return maxFreq;}
    
    
private:
    ofSoundPlayer           drumSounds[10];
    int                     framePlay;
    int                     drumCount;
    float                   minFreq;
    float                   maxFreq;
};