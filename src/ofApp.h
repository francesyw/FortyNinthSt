#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxSyphon.h"
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
    
        ofTexture image;
        ofVideoPlayer video;
//        ofVideoGrabber camera;
    
        ofxGuiGroup mixerGroup;
        ofxFloatSlider imageAlpha, videoAlpha; //, cameraAlpha;
        ofxToggle kenabled;
        ofxIntSlider ksectors;
        ofxFloatSlider kangle, kx, ky;

        ofFbo fbo;
        ofShader shader;
    
//        ofSpherePrimitive sphere;
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
    
        void audioIn(float *input, int bufferSize, int nChannels);
        float soundLevel;
    
        ofxToggle automate;
        
        ofxSyphonServer mainOutputSyphonServer;
        ofxSyphonServer individualTextureSyphonServer;
        
        ofxSyphonClient mClient;
    
        ofxJSONElement json;
    
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


