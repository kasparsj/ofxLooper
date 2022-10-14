#pragma once

#include "FramesBuffer.h"
#include "ofFbo.h"

class ofxLooper {
public:
    void setup(float maxDuration = 10, int fps = 30, float playSpeed = 2.0, int bytesRate = 2000000 * 40);
    void swapBuffers(bool swapBuffers = false);
    void addFrame(ofPixels& pixels);
    void update();
    void draw(float x, float y);
    void draw(float x, float y, float w, float h);
    void draw(const glm::vec2 & point);
    void draw(ofRectangle rect);
    void draw(const glm::vec2 & point, float w, float h);
    int getFps() const {
        return fps;
    }
    void setFps(int value) {
        fps = value;
        updateFps();
    }
    float getMaxDuration() const {
        return maxDuration;
    }
    void setMaxDuration(float value) {
        maxDuration = value;
    }
    float getPlaySpeed() const {
        return playSpeed;
    }
    void setPlaySpeed(float value) {
        playSpeed = value;
        updateFps();
    }
    int getWidth() const {
        return fbo.getWidth();
    }
    int getHeight() const {
        return fbo.getHeight();
    }
    ofFbo& getFbo() {
        return fbo;
    }

private:
    void updateFps() {
        recFps = fps / playSpeed;
        framesBuffer1.setCurrentFPS(recFps);
        framesBuffer2.setCurrentFPS(recFps);
    }
    
    int fps;
    int recFps;
    float playSpeed;
    float loopStartTime = 0;
    float maxDuration;
    
    FramesBuffer framesBuffer1;
    FramesBuffer framesBuffer2;
    FramesBuffer* recBuffer;
    FramesBuffer* playBuffer;
    
    ofFbo fbo;
};
