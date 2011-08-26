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
    
private:
    ofSoundPlayer drumSounds[10];
};