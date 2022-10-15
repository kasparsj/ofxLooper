#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
    camWidth = 1280;  // try to grab at this size.
    camHeight = 720;

    vector<ofVideoDevice> devices = vidGrabber.listDevices();
    for(size_t i = 0; i < devices.size(); i++){
        if(devices[i].bAvailable){
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName;
        }else{
            ofLogNotice() << devices[i].id << ": " << devices[i].deviceName << " - unavailable ";
        }
    }
    
    vidGrabber.setDeviceID(0);
    vidGrabber.setDesiredFrameRate(30);
    vidGrabber.setup(camWidth, camHeight);

    ofSetFrameRate(60);
    ofSetVerticalSync(true);

    looper.setup(2, 30, 2.0);
    
    receiver.setup(PORT);
    
    myGlitch.setup(&looper.getFbo());
}


//--------------------------------------------------------------
void ofApp::update(){
    bool forceSwap;
    while(receiver.hasWaitingMessages()){
        ofxOscMessage m;
        receiver.getNextMessage(m);
        ofLog() << m;
        if (m.getAddress() == "/max_duration") {
            looper.setMaxDuration(m.getArgAsFloat(0));
        }
        if (m.getAddress() == "/swap_buffers"){
            forceSwap = true;
        }
        if (m.getAddress() == "/set_fps") {
            int camFrameRate = m.getArgAsInt(0);
            vidGrabber.setDesiredFrameRate(camFrameRate);
            looper.setFps(camFrameRate);
        }
        if (m.getAddress() == "/set_speed") {
            looper.setPlaySpeed(m.getArgAsFloat(0));
        }
        if (m.getAddress() == "/toggle_glitch") {
            for (int i=0; i<m.getNumArgs(); i++) {
                myGlitch.toggleFx(static_cast<ofxPostGlitchType>(m.getArgAsInt(i)));
            }
        }
        if (m.getAddress() == "/set_glitch") {
            for (int i=0; i<m.getNumArgs(); i+=2) {
                myGlitch.setFx(static_cast<ofxPostGlitchType>(m.getArgAsInt(i)), m.getArgAsBool(i+1));
            }
        }
    }
    looper.swapBuffers(forceSwap);
    vidGrabber.update();
    if (vidGrabber.isFrameNew()){
        looper.addFrame(vidGrabber.getPixels());
    }
    looper.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(0);
    ofSetColor(255);
    
    if (showWebcam) {
        vidGrabber.draw(
                        flipHorizontal ? vidGrabber.getWidth() : 0,
                        flipVertical ? vidGrabber.getHeight() : 0,
                        flipHorizontal ? -vidGrabber.getWidth() : vidGrabber.getWidth(),
                        flipVertical ? -vidGrabber.getHeight() : vidGrabber.getHeight()
                        );
    }
    else {
        myGlitch.generateFx();

        ofRectangle rect( 0.f, 0.f, ofGetWidth(), ofGetHeight());
        ofRectangle r( 0.f, 0.f, looper.getWidth(), looper.getHeight());
        r.scaleTo(rect, OF_SCALEMODE_FIT);
        looper.draw(ofRectangle(
                                flipHorizontal ? r.width : 0,
                                flipVertical ? r.height : 0,
                                flipHorizontal ? -r.width : r.width,
                                flipVertical ? -r.height : r.height
                                ));
    }
    
    if (showFrameRate) {
        ofDrawBitmapStringHighlight(ofToString(ofGetFrameRate()), glm::vec2(20, 20));
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 's' || key == 'S'){
        vidGrabber.videoSettings();
    }
    if (key == 'd') {
        showFrameRate = !showFrameRate;
    }
    if (key == 'f') {
        ofToggleFullscreen();
    }
    if (key == 'w') {
        showWebcam = !showWebcam;
    }
    if (key == 'h') {
        flipHorizontal = !flipHorizontal;
    }
    if (key == 'v') {
        flipVertical = !flipVertical;
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y){
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
void ofApp::mouseEntered(int x, int y){
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y){
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){
}
