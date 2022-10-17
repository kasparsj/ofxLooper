# ofxLooper

ofxLooper is a frames buffer that can play and record at the same time. 
Once the recording buffer is filled, it is swapped with the play buffer.

## Usage

See [example](https://github.com/kasparsj/ofxLooper/tree/main/example).

```cpp
void ofApp::setup(){
    int fps = 60;
    ofSetFrameRate(fps);

    int maxDuration = 2; // seconds
    float speed = 2.0; // 2x play speed
    looper.setup(maxDuration, fps/speed, speed);
}


//--------------------------------------------------------------
void ofApp::update(){
    // will swap every 2 seconds
    looper.swapBuffers();
    ofPixels& pixels = getPixels();
    looper.addFrame(pixels);
    looper.update();
}

//--------------------------------------------------------------
void ofApp::draw() {
    ofBackground(0);
    ofSetColor(255);
    
    looper.draw(0, 0);
}

ofPixels& ofApp::getPixels() {
  // todo: implement get & return your pixels from Webcam or video, or etc. e.g.:
  // vidGrabber.update();
  // vidGrabber.getPixels();
}
```

## FramesBuffer

This addon is based on FramesBuffer from:
https://forum.openframeworks.cc/t/live-sampling-video-grabber/35655
