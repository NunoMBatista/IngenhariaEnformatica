#include "ofMain.h"
#include "ofApp.h"
#include "global.h"

int main( ){
	//Use ofGLFWWindowSettings for more options like multi-monitor fullscreen
	ofGLWindowSettings settings;
	//settings.setSize(1024, 768);
	settings.setSize(1536, 1024);

	settings.windowMode = OF_FULLSCREEN; //can also be OF_FULLSCREEN

	auto window = ofCreateWindow(settings);

	ofRunApp(window, make_shared<ofApp>());
	ofRunMainLoop();

}
