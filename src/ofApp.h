#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxJSON.h"

class ofApp : public ofBaseApp{

	public:
        bool showGui;
        bool showfullScreen;
        ofxPanel gui;
        ofxIntSlider countX;
        ofxIntSlider countY;
        ofxFloatSlider stepX;
        ofxFloatSlider stepY, twistY, pinchY;
        ofxFloatSlider twistX;
    
        ofxGuiGroup globalGroup;
        ofxFloatSlider Scale;
        ofxFloatSlider Rotate;
        ofxFloatSlider Background;
    
        ofxGuiGroup primGroup;
        ofxFloatSlider shiftY, rotate;
        ofxVec2Slider size;
        ofxColorSlider color;
        ofxToggle filled, type;
    
        ofxGuiGroup mixerGroup;
        ofxToggle kenabled;
        ofxIntSlider ksectors;
        ofxFloatSlider kangle, kx, ky;

        ofFbo fbo;
        ofShader shader;
    
        ofPlanePrimitive videoPlane;
        ofEasyCam cam;
        ofLight light;
        ofMaterial material;    
        ofFbo fbo2;
    
        ofxFloatSlider show2d, show3d;
        ofFbo fbo3d;
    
        ofxFloatSlider rad, deform, deformFreq, extrude;
        vector<ofPoint> vertices0;
    
        float phase = 0;
        float frequency = 0.1;
    
        ofxToggle automate;
    
        ofxJSONElement json;
        float temp[45];
        float pressure[45];
        float humidity[45];
        float windSpeed[45];
        float windDeg[45];
    
        void setup();
		void update();
		void draw();
        void exit();
    
        void stripePattern();
        void matrixPattern();
        void draw2d();
        void draw3d();
    
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
		
};


