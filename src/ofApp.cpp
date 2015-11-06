#include "ofApp.h"

int W = 100;		//Grid size
int H = 100;

//--------------------------------------------------------------
void ofApp::setup(){
    showGui = true;
    showfullScreen = false;
    int screenWidth = 1920;
    int screenHeight = 1080;
    
    ofSetWindowTitle( "49th St" );
    ofSetWindowShape( 1920, 1080 );
    ofSetFrameRate( 60 );
    ofBackground( ofColor::black );

    gui.setup( "Parameters", "settings.xml" );
    gui.add( countX.setup( "countX", 50, 0, 200 ) );
    gui.add( stepX.setup( "stepX", 20, 0, 200 ) );
    gui.add( twistX.setup( "twistX", 5, -45, 45 ) );
    gui.add( countY.setup( "countY", 0, 0, 50) );
    gui.add( stepY.setup( "stepY", 20, 0, 200 ) );
    gui.add( twistY.setup( "twistY", 0, -30, 30 ) );
    gui.add( pinchY.setup( "pinchY", 0, 0, 1 ) );
    
    globalGroup.setup( "Global" );
    globalGroup.add( Scale.setup( "Scale", 1, 0.0, 1 ) );
    globalGroup.add( Rotate.setup( "Rotate", 0, -180, 180 ) );
    globalGroup.add( Background.setup("Background",255,0, 255));
    gui.add( &globalGroup );
    
    primGroup.setup( "Primitive" );
    primGroup.add( shiftY.setup("shiftY",0.0,-1000.0,1000.0 ) );
    primGroup.add( rotate.setup("rotate",0.0,-180.0,180.0 ) );
    primGroup.add( size.setup( "size", ofVec2f(6,6), ofVec2f(0,0), ofVec2f(20,20) ) );
    primGroup.add( color.setup( "color", ofColor::black, ofColor(1,1,0,0), ofColor::white ) );
    primGroup.add( filled.setup( "filled", false ) );
    primGroup.add( type.setup( "type", false ) );
    gui.add( &primGroup );
    
    mixerGroup.setup( "Mixer" );
    mixerGroup.setHeaderBackgroundColor( ofColor::darkRed );
    mixerGroup.setBorderColor( ofColor::darkRed );
    mixerGroup.add( imageAlpha.setup( "image", 100,0,255 ) );
    mixerGroup.add( videoAlpha.setup( "video", 200,0,255 ) );
//    mixerGroup.add( cameraAlpha.setup( "camera", 100,0,255 ) );
    shader.load( "kaleido" );
    mixerGroup.add( kenabled.setup( "kenabled", true ) );
    mixerGroup.add( ksectors.setup( "ksectors", 10, 1, 100 ) );
    mixerGroup.add( kangle.setup( "kangle", 0, -180, 180 ) );
    mixerGroup.add( kx.setup( "kx", 0.5, 0, 1 ) );
    mixerGroup.add( ky.setup( "ky", 0.5, 0, 1 ) );
    mixerGroup.add( show2d.setup("show2d", 255, 0, 255) );
    mixerGroup.add( show3d.setup("show3d", 255, 0, 255) );
    mixerGroup.add( rad.setup("rad", 250, 0, 500) );
    mixerGroup.add( deform.setup("deform", 0.3, 0, 1.5) );
    mixerGroup.add( deformFreq.setup("deformFreq", 3, 0, 10) );
    mixerGroup.add( extrude.setup("extrude", 0, 0, 300 ) );
    mixerGroup.add( automate.setup( "automate", true ) );
    gui.minimizeAll();
    gui.add( &mixerGroup );
    
    gui.loadFromFile( "settings.xml" );
    
    ofLoadImage( image, "collage.png" );
//    video.loadMovie( "flowing.mp4" );
//    video.play();
    
    fbo.allocate( screenWidth, screenHeight, GL_RGB );
    
    videoPlane.set(screenWidth, screenHeight);
    videoPlane.setResolution(100, 100);
    vertices0 = videoPlane.getMesh().getVertices();
    fbo2.allocate( screenWidth, screenHeight, GL_RGB );
    float w = fbo2.getWidth();
    float h = fbo2.getHeight();
    videoPlane.mapTexCoords(0, h, w, 0);
    videoPlane.rotate(0, 0, 1, 0);
    
    fbo3d.allocate( screenWidth, screenHeight, GL_RGBA );
    
    soundLevel = 0;
    ofSoundStreamSetup( 0, 1, 44100, 128, 4 );
    
    // *** syphon ***
    mClient.setup();
    mClient.set("","EpocCam Viewer Pro");
    
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
        kx = ofMap(value, -1, 1, 0.45, 0.55);
        
        float phase1 = 0.1 * ofGetElapsedTimef();
        deform = ofMap(ofNoise( phase1 ), 0, 1, 0.01, 0.08);
        deformFreq = ofMap(ofNoise( phase1, 33.3 ), 0, 1, 5, 10);
        

        double level = 0;
        level += (soundLevel*2);
//        cout << level << endl;
        float newExtrude = ofMap(level, 0, 0.3, 50, 300, true);
        extrude = extrude + 0.1 * (newExtrude-extrude);
//        float newRad = ofMap( level, 0, 1, 100, 300, true );
//        rad = rad + 0.5 * (newRad-rad);
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    fbo.begin();
    draw2d();
    fbo.end();
    
    ofSetColor( 255 );
    fbo2.begin();
    if ( kenabled ) {
        shader.begin();
        shader.setUniform1i( "ksectors", ksectors );
        shader.setUniform1f( "kangleRad", ofDegToRad(kangle) );
        shader.setUniform2f( "kcenter", kx*ofGetWidth(), ky*ofGetHeight() );
        shader.setUniform2f( "screenCenter", 0.5*ofGetWidth(), 0.5*ofGetHeight() );
    }
    fbo.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    if ( kenabled ) shader.end();
    fbo2.end();
    
    fbo3d.begin();
    ofBackground( 0, 0 );
    draw3d();
    fbo3d.end();
    
    ofBackground( 0 );
    ofSetColor( 255, show2d );
    fbo2.draw( 0, 0 );
    ofSetColor( 255, show3d );
    fbo3d.draw( 0, 0 );
    
//    ofSetColor( 255 );
//    ofDrawBitmapString( ofToString( ofGetFrameRate() ), 250, 20 );
    
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
}


//**************************************************************
// TODO: replace primitives with ofMesh() / ofVboMesh() / glDraw()
void ofApp::stripePattern() {
    ofColor baseColor = color;
    ofSetLineWidth( 1.0 );
    if ( filled ) ofFill();
    else ofNoFill();
    
    for (int i = -countX; i < countX; i++) {
        ofPushMatrix();
        ofTranslate( i * stepX, 0 );
        ofRotate( i * twistX );

        ofTranslate( 0, shiftY );
        ofRotate( rotate );
        ofScale( size->x, size->y );
        ofSetColor( abs(i)*3%baseColor.r, abs(i)*5%baseColor.g, baseColor.b, baseColor.a );
        if ( type ) ofRect( -50, -50, 100, 100 );
        else ofTriangle( 0, 0, -50, 100, 50, 100 );
        ofPopMatrix();
    }
}

//**************************************************************
void ofApp::matrixPattern() {
    for (int y = -countY; y <= countY; y++) {
        ofPushMatrix();
        //---------------------
        if ( countY > 0 ) {
            float scl = ofMap( y, -countY, countY, 1-pinchY, 1 );
            ofScale( scl, scl );
        }
        ofTranslate( 0, y * stepY );
        ofRotate( y * twistY );
        stripePattern();
        //---------------------
        ofPopMatrix();
    }
}

//**************************************************************
void ofApp::draw2d() {
    ofBackground( Background );
    
    ofDisableSmoothing();
    ofEnableBlendMode( OF_BLENDMODE_ADD );
    ofSetColor( 255, imageAlpha );
    image.draw( 0, 0, ofGetWidth(), ofGetHeight() );
    ofSetColor( 255, videoAlpha );
    mClient.draw( 0, 0, ofGetWidth(), ofGetHeight() );
//    if ( camera.isInitialized() ) {
//        ofSetColor( 255, cameraAlpha );
//        camera.draw( 0, 0, ofGetWidth(), ofGetHeight() );
//    }
    ofEnableAlphaBlending();
    ofEnableSmoothing();
    
    ofPushMatrix();
    ofTranslate( ofGetWidth() / 2, ofGetHeight() / 2 );
    float Scl = pow( Scale, 4.0f );
    ofScale( Scl, Scl );
    ofRotate( Rotate );
    matrixPattern();
    ofPopMatrix();
}

//**************************************************************
void ofApp::draw3d() {
    fbo2.getTextureReference().bind();
    
    light.setPosition(ofGetWidth()/2, ofGetHeight()/2, 600);
    light.enable();
    material.begin();
    ofEnableDepthTest();
    
    cam.begin();
//    cam.enableOrtho();
//    light.setPosition(0, 0, 600);
//    light.enable();
//    light.draw();
    
      /* camera movement */
//    float time = ofGetElapsedTimef();
//    float longitude = 10*time;
//    float latitude = 10*sin(time*0.8);
//    float radius = 600 + 50*sin(time*0.4);
//    cam.orbit( longitude, latitude, radius, ofPoint(0,0,0) );
    
    ofSetColor( ofColor::white );
    videoPlane.drawWireframe();
//    sphere.draw();
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
//    if ( key == 'c' ) {
//        camera.setDeviceID( 0 );
//        camera.setDesiredFrameRate( 30 );
//        camera.initGrabber( 1280, 720 );
//    }
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
