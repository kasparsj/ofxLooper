#include "ofxLooper.h"
#include "ofGraphics.h"

void ofxLooper::setup(int width, int height, int fps) {
    setFps(fps);
    
    framesBuffer1.setLoop(true);
    framesBuffer1.setCurrentFPS(recFps);
    framesBuffer1.setCurrentBytesRate(2000000 * 40);

    framesBuffer2.setLoop(true);
    framesBuffer2.setCurrentFPS(recFps);
    framesBuffer2.setCurrentBytesRate(2000000 * 40);
    
    recBuffer = &framesBuffer1;
    playBuffer = &framesBuffer2;
    
    fbo.allocate(width, height);
}

void ofxLooper::swapBuffers(bool force) {
    if (recBuffer->getDuration() >= maxDuration || force) {
        if (recBuffer == &framesBuffer1) {
            recBuffer = &framesBuffer2;
            playBuffer = &framesBuffer1;
        }
        else {
            recBuffer = &framesBuffer1;
            playBuffer = &framesBuffer2;
        }
        recBuffer->clear();
        playBuffer->play(true);
        loopStartTime = ofGetElapsedTimef();
    }
}

void ofxLooper::addFrame(ofPixels& pixels) {
    float speed = (fps / recFps);
    if (!recBuffer->addFrame(pixels, (ofGetElapsedTimef() - loopStartTime) / speed)) {
        ofLog() << "not enough memory";
    }
}

void ofxLooper::update() {
    playBuffer->update();
    
    fbo.begin();
    ofClear(0, 0, 0, 255);
    ofSetColor(255);
    playBuffer->draw();
    fbo.end();
}

void ofxLooper::draw(float x, float y) {
    fbo.draw(x, y);
}

void ofxLooper::draw(float x, float y, float w, float h) {
    fbo.draw(x, y, w, h);
}

void ofxLooper::draw(const glm::vec2 & point) {
    fbo.draw(point);
}

void ofxLooper::draw(ofRectangle rect) {
    fbo.draw(rect);
}

void ofxLooper::draw(const glm::vec2 & point, float w, float h) {
    fbo.draw(point, w, h);
}
