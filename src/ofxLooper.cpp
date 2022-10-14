#include "ofxLooper.h"
#include "ofGraphics.h"

void ofxLooper::setup(float maxDuration, int fps, float playSpeed, int bytesRate) {
    setMaxDuration(maxDuration);
    setFps(fps);
    setPlaySpeed(playSpeed);
    
    framesBuffer1.setLoop(true);
    framesBuffer1.setCurrentFPS(recFps);
    framesBuffer1.setCurrentBytesRate(bytesRate);

    framesBuffer2.setLoop(true);
    framesBuffer2.setCurrentFPS(recFps);
    framesBuffer2.setCurrentBytesRate(bytesRate);
    
    recBuffer = &framesBuffer1;
    playBuffer = &framesBuffer2;
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
    if (!fbo.isAllocated()) {
        fbo.allocate(pixels.getWidth(), pixels.getHeight());
    }
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
