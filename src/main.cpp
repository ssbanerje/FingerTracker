#include "FingerTracker.h"
#include "ofAppGlutWindow.h"
#include "Constants.h"

int main() {
	ofAppGlutWindow window;
	ofSetupOpenGL(&window, WIDTH, HEIGHT, OF_WINDOW);
	ofRunApp(new FingerTracker());
}
