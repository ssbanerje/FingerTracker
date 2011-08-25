#pragma once

#include "ofMain.h"
#include "ofSoundPlayer.h"

using namespace std;

class DrumKit {
public:
    DrumKit();
    ~DrumKit() {}
    
    void draw();
private:
    ofSoundPlayer drumSounds[10];
};