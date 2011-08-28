#include "FingerTracker.h"

//--------------------------------------------------------------
inline void FingerTracker::fadeToColor(float r, float g, float b, float speed) {
    glColor4f(r, g, b, speed);
	ofRect(0, 0, ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void FingerTracker::addToFluid( Vec2f pos, Vec2f vel, bool addColor, bool addForce ) {
    float speed = vel.x * vel.x  + vel.y * vel.y * getWindowAspectRatio() * getWindowAspectRatio();
    if(speed > 0) {
		pos.x = constrain(pos.x, 0.0f, 1.0f);
		pos.y = constrain(pos.y, 0.0f, 1.0f);
        const float colorMult = 100;
        const float velocityMult = 30;
        int index = fluidSolver.getIndexForPos(pos);
		if(addColor) {
			Color drawColor( CM_HSV, ( getElapsedFrames() % 360 ) / 360.0f, 1, 1 );
			fluidSolver.addColorAtIndex(index, drawColor * colorMult);
		}
		if(addForce)
			fluidSolver.addForceAtIndex(index, vel * velocityMult);
    }
}