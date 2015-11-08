#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    showGui = true;
    showfullScreen = false;
    int screenWidth = 1920;
    int screenHeight = 1080;
    
    ofSetWindowTitle( "49th St" );
    ofSetWindowShape( 1920, 1080 );
    ofSetFrameRate( 60 );
    ofBackground( ofColor::grey );

    gui.setup( "Parameters", "settings.xml" );
    
    globalGroup.setup( "Global" );
    globalGroup.add( Scale.setup( "Scale", 1, 0.0, 1 ) );
    globalGroup.add( Rotate.setup( "Rotate", 0, -180, 180 ) );
    globalGroup.add( Background.setup("Background",255,0, 255));
    gui.add( &globalGroup );
    
    mixerGroup.setup( "Mixer" );
    mixerGroup.setHeaderBackgroundColor( ofColor::darkRed );
    mixerGroup.setBorderColor( ofColor::darkRed );
    mixerGroup.add( videoAlpha.setup( "video", 200,0,255 ) );
//    mixerGroup.add( cameraAlpha.setup( "camera", 100,0,255 ) );
    mixerGroup.add( show2d.setup("show2d", 255, 0, 255) );
    mixerGroup.add( show3d.setup("show3d", 255, 0, 255) );
    mixerGroup.add( rad.setup("rad", 1, 1, 7) );
    mixerGroup.add( deform.setup("deform", 0.3, 0, 1.5) );
    mixerGroup.add( deformFreq.setup("deformFreq", 3, 0, 10) );
    mixerGroup.add( extrude.setup("extrude", 0, 0, 300 ) );
    mixerGroup.add( automate.setup( "automate", false ) );
    gui.minimizeAll();
    gui.add( &mixerGroup );
    
    gui.loadFromFile( "settings.xml" );
    
    fbo.allocate( screenWidth, screenHeight, GL_RGB );
    
    videoPlane.set(screenWidth, screenHeight);
    videoPlane.setResolution(80, 60);
    vertices0 = videoPlane.getMesh().getVertices();
    fbo2.allocate( screenWidth, screenHeight, GL_RGB );
    float w = fbo2.getWidth();
    float h = fbo2.getHeight();
    videoPlane.mapTexCoords(0, h, w, 0);
    videoPlane.rotate(0, 0, 1, 0);
    
    fbo3d.allocate( screenWidth, screenHeight, GL_RGBA );
    
    soundLevel = 0;
    ofSoundStreamSetup( 0, 1, 44100, 128, 4 );
    
    
    // *** kinect ***
    kinect.setRegistration(true);
    kinect.init();
    kinect.open();
    
    if(kinect.isConnected()) {
        ofLogNotice() << "sensor-emitter dist: " << kinect.getSensorEmitterDistance() << "cm";
        ofLogNotice() << "sensor-camera dist:  " << kinect.getSensorCameraDistance() << "cm";
        ofLogNotice() << "zero plane pixel size: " << kinect.getZeroPlanePixelSize() << "mm";
        ofLogNotice() << "zero plane dist: " << kinect.getZeroPlaneDistance() << "mm";
    }
    
//    colorImg.allocate(kinect.width, kinect.height);
//    grayImage.allocate(kinect.width, kinect.height);
//    grayThreshNear.allocate(kinect.width, kinect.height);
//    grayThreshFar.allocate(kinect.width, kinect.height);
    
    nearThreshold = 230;
    farThreshold = 70;
//    bThreshWithOpenCV = true;
    angle = 0;
    kinect.setCameraTiltAngle(angle);
    
    
    // *** video plane ***
    
    // *** weather api ***
//    string url = "http://api.openweathermap.org/data/2.5/weather?id=5110302&units=metric&APPID=80b894b8014884a432cb5bea1fe2c422";
//    // Now parse the JSON
//    bool parsingSuccessful = json.open(url);
//    if (parsingSuccessful)
//    {
//        ofLogNotice("ofApp::setup") << json.getRawString(true);
//    } else {
//        ofLogNotice("ofApp::setup") << "Failed to parse JSON.";
//    }
    
}

//--------------------------------------------------------------
void ofApp::update(){
//    video.update();
//    if ( camera.isInitialized() ) camera.update();
    
    vector<ofPoint> &vertices = videoPlane.getMesh().getVertices();
//    for (int i=0; i<vertices.size(); i++) {
//        ofPoint v = vertices0[i];
//        v.normalize();
//        float sx = sin( v.x * deformFreq );
//        float sy = sin( v.y * deformFreq );
//        float sz = sin( v.z * deformFreq );
//        v.x += sy * sz * deform;
//        v.y += sx * sz * deform;
//        v.z += sx * sy * deform;
//        v *= rad;
//        vertices[i] = v;
//    }
    
    ofPixels pixels;
    fbo2.readToPixels(pixels);
    for (int i=0; i<vertices.size(); i++) {
        ofVec2f t = videoPlane.getMesh().getTexCoords()[i];
        t.x = ofClamp( t.x, 0, pixels.getWidth()-1 );
        t.y = ofClamp( t.y, 0, pixels.getHeight()-1 );
        float br = pixels.getColor(t.x, t.y).getBrightness();
        vertices[i] = vertices0[i] + br / 255.0 * extrude;
    }
    
    if ( automate ) {
        /* automation */
        float dt = 1.0 / 60.0; // 60 - framerate
        phase += dt * frequency * M_TWO_PI;
    //    float phase = 0.1 * ofGetElapsedTimef() * M_TWO_PI;
        float value = sin( phase );
//        kx = ofMap(value, -1, 1, 0.45, 0.55);
        
        float phase1 = 0.1 * ofGetElapsedTimef();
        deform = ofMap(ofNoise( phase1 ), 0, 1, 0.01, 0.08);
        deformFreq = ofMap(ofNoise( phase1, 33.3 ), 0, 1, 5, 10);
        

        double level = 0;
        level += (soundLevel*2);
//        cout << level << endl;
        float newExtrude = ofMap(level, 0, 0.3, 50, 300, true);
        extrude = extrude + 0.1 * (newExtrude-extrude);
//        float newRad = ofMap( level, 0, 0.02, 1, 7, true );
//        rad = floor(rad + 0.3 * (newRad-rad));
    }
    
    
    // *** kinect ***
    kinect.update();
//    if(kinect.isFrameNew()) {
//        
//        // load grayscale depth image from the kinect source
//        grayImage.setFromPixels(kinect.getDepthPixels(), kinect.width, kinect.height);
//        
//        // we do two thresholds - one for the far plane and one for the near plane
//        // we then do a cvAnd to get the pixels which are a union of the two thresholds
//        grayThreshNear = grayImage;
//        grayThreshFar = grayImage;
//        grayThreshNear.threshold(nearThreshold, true);
//        grayThreshFar.threshold(farThreshold);
//        cvAnd(grayThreshNear.getCvImage(), grayThreshFar.getCvImage(), grayImage.getCvImage(), NULL);
//        
//        // update the cv images
//        grayImage.flagImageChanged();
//        
//        // find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
//        // also, find holes is set to true so we will get interior contours as well....
////        contourFinder.findContours(grayImage, 10, (kinect.width*kinect.height)/2, 20, false);
//    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground( Background );
    ofSetColor(255, 255, 255);
//    kinect.draw(0, 0, ofGetWidth(), ofGetHeight());
//    draw3d();
    
    fbo.begin();
    ofClear(255,255,255, 0);
    draw2d();
    fbo.end();
    
    ofSetColor( 255 );
    fbo2.begin();
    ofClear(255,255,255, 0);
    fbo.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    fbo2.end();
    
    fbo3d.begin();
    ofClear(255,255,255, 0);
    draw3d();
    fbo3d.end();
    
//    ofSetColor( 255, show2d );
//    fbo2.draw( 0, 0 );
    
    ofSetColor( 255, show3d );
    fbo3d.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    
    ofSetColor( 255 );
    ofDrawBitmapString( ofToString( ofGetFrameRate() ), 250, 20 );
    
    // json test
//    ofSetColor(255);
//    string temp = ofToString(json["main"]["temp"].asFloat());
//    string pressure = ofToString(json["main"]["pressure"].asFloat());
//    string humidity = ofToString(json["main"]["humidity"].asInt());
//    string windSpeed = ofToString(json["wind"]["speed"].asFloat());
//    string windDeg =  ofToString(json["wind"]["deg"].asFloat());
//    string weatherMain =  json["weather"][0]["main"].asString();
//    string text = "temp: " + temp + "\n" + "pressure: " + pressure + "\n" + "humidity: " + humidity + "\n" +
//                  "wind speed: " + windSpeed + " | " + "wind degree: " + windDeg + "\n" + "weather: " + weatherMain;
//    ofDrawBitmapString(text, 100, 100);

    
    if ( showGui ) gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit(){
    gui.saveToFile( "settings.xml" );
    kinect.setCameraTiltAngle(0); // zero the tilt on exit
    kinect.close();
}


//**************************************************************
void ofApp::draw2d() {
//    ofBackground( Background );
    
//    ofDisableSmoothing();
//    ofEnableBlendMode( OF_BLENDMODE_ADD );
//    ofSetColor( 255, videoAlpha );
    kinect.draw( 0, 0, ofGetWidth(), ofGetHeight() );
//    if ( camera.isInitialized() ) {
//        ofSetColor( 255, cameraAlpha );
//        camera.draw( 0, 0, ofGetWidth(), ofGetHeight() );
//    }
//    ofEnableAlphaBlending();
//    ofEnableSmoothing();
}

//**************************************************************
void ofApp::draw3d() {
    fbo2.getTextureReference().bind();
    
    light.setPosition(ofGetWidth()/2, ofGetHeight()/2, 600);
    light.enable();
    material.begin();
    ofEnableDepthTest();
    
    cam.begin();
    ofSetColor( ofColor::white );
    videoPlane.drawWireframe();
    cam.end();
    
    ofDisableDepthTest();
    material.end();
    light.disable();
    ofDisableLighting();
    
    fbo2.getTextureReference().unbind();
}


//**************************************************************
void ofApp::audioIn(float *input, int bufferSize, int nChannels){
    double v = 0;
    for (int i=0; i<bufferSize; i++) {
        v += input[i] * input[i];
    }
    v = sqrt( v / bufferSize );
    soundLevel = v;
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if ( key == 'z' ) showGui = !showGui;
    if ( key == '`' ) {
        showfullScreen = !showfullScreen;
        ofSetFullscreen( showfullScreen );
    }
    if ( key == OF_KEY_RETURN ) {
        ofSaveScreen( "screenshot" + ofToString(ofRandom(0, 1000), 0) + ".png" );
    }
    if ( key == 's' ) {
        ofFileDialogResult res;
        res = ofSystemSaveDialog( "preset.xml", "Saving Preset" );
        if ( res.bSuccess ) gui.saveToFile( res.filePath );
    }
    if ( key == 'l' ) {
        ofFileDialogResult res;
        res = ofSystemLoadDialog( "Loading Preset" );
        if ( res.bSuccess ) gui.loadFromFile( res.filePath );
    }
    
    switch (key) {
        case '>':
        case '.':
            farThreshold ++;
            if (farThreshold > 255) farThreshold = 255;
            break;
            
        case '<':
        case ',':
            farThreshold --;
            if (farThreshold < 0) farThreshold = 0;
            break;
            
        case '+':
        case '=':
            nearThreshold ++;
            if (nearThreshold > 255) nearThreshold = 255;
            break;
            
        case '-':
            nearThreshold --;
            if (nearThreshold < 0) nearThreshold = 0;
            break;
            
        case 'w':
            kinect.enableDepthNearValueWhite(!kinect.isDepthNearValueWhite());
            break;
            
        case 'o':
            kinect.setCameraTiltAngle(angle); // go back to prev tilt
            kinect.open();
            break;
            
        case 'c':
            kinect.setCameraTiltAngle(0); // zero the tilt
            kinect.close();
            break;
            
        case '1':
            kinect.setLed(ofxKinect::LED_GREEN);
            break;
            
        case '2':
            kinect.setLed(ofxKinect::LED_YELLOW);
            break;
            
        case '3':
            kinect.setLed(ofxKinect::LED_RED);
            break;
            
        case '4':
            kinect.setLed(ofxKinect::LED_BLINK_GREEN);
            break;
            
        case '5':
            kinect.setLed(ofxKinect::LED_BLINK_YELLOW_RED);
            break;
            
        case '0':
            kinect.setLed(ofxKinect::LED_OFF);
            break;
            
        case OF_KEY_UP:
            angle++;
            if(angle>30) angle=30;
            kinect.setCameraTiltAngle(angle);
            break;
            
        case OF_KEY_DOWN:
            angle--;
            if(angle<-30) angle=-30;
            kinect.setCameraTiltAngle(angle);
            break;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    if ( showGui && x < 250 ) cam.disableMouseInput();
    else cam.enableMouseInput();
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    ofSetWindowShape(w, h);
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
