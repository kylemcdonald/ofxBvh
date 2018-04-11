#include "ofMain.h"
#include "ofxBvh.h"

float getHeight(ofxBvh& bvh) {
    float height = 0;
    for (auto& joint : bvh.getJoints()) {
        glm::vec3 cur = joint->getPosition();
        height = std::max(height, cur.y);
    }
    return height;
}

class ofApp : public ofBaseApp {
public:
    ofxBvh bvh;
    ofEasyCam cam;
    ofLight light;
    float height;
    vector<string> filenames;
    string filename;
    int nextFilenameIndex = 0;
    unsigned int previousFrame = 0;
    unsigned int skippedFrames = 0;
    unsigned int totalFrames = 0;
    
    void setup() {
        ofBackground(0);
        ofSetFrameRate(60);
        ofSetVerticalSync(true);
    }
    void resetSkippedFrames() {
        skippedFrames = 0;
        totalFrames = 0;
        previousFrame = bvh.getFrame();
    }
    void loadNextFile() {
        if (filenames.empty()) return;
        filename = filenames[nextFilenameIndex];
        nextFilenameIndex = (nextFilenameIndex + 1) % filenames.size();
        bvh = ofxBvh(filename);
        bvh.update();
        height = getHeight(bvh);
        bvh.play();
        bvh.setLoop(true);
        resetSkippedFrames();
    }
    void dragged(ofDragInfo& drag) {
        if (drag.files.empty()) return;
        vector<string> valid;
        for (auto fn : drag.files) {
            if (ofFile(fn).getExtension() != "bvh") continue;
            valid.push_back(fn);
        }
        if (!valid.empty()) {
            filenames = valid;
            nextFilenameIndex = 0;
            loadNextFile();
        }
    }
    void update() {
        if (!bvh.ready()) return;
        if (!bvh.isPlaying()) {
            bvh.setPosition((float) mouseX / ofGetWidth());
            resetSkippedFrames();
        }
        unsigned int curFrame = bvh.getFrame();
        if (curFrame != previousFrame && curFrame > previousFrame) {
            unsigned int diff = curFrame - previousFrame;
            skippedFrames += diff - 1;
            totalFrames += diff;
        }
        previousFrame = curFrame;
        bvh.update();
    }
    void draw() {
        float w = ofGetWidth(), h = ofGetHeight();
        if (!bvh.ready()) {
            ofDrawBitmapString("Drop a file to play.", w/2, h/2);
            return;
        }
        ofSetColor(255);
        cam.begin();
        ofPushMatrix();
        float scale = 0.75 * h / height;
        ofScale(scale, scale, scale);
        ofTranslate(0, -height/2);
        ofEnableLighting();
        ofEnableDepthTest();
        light.enable();
        bvh.draw();
        light.disable();
        ofDisableDepthTest();
        ofDisableLighting();
        ofPopMatrix();
        cam.end();
        
        int skippedPercent = totalFrames > 0 ? (100 * skippedFrames) / totalFrames : 0;
        stringstream topText;
        topText
        << filename << endl
        << "Frame: " << bvh.getFrame() << "/" << bvh.getNumFrames() << " @ " << bvh.getFrameRate() << "fps" << endl
        << "Skipped: " << skippedFrames << " / " << totalFrames << " = " << skippedPercent << "%" << endl
        << "Time: " << round(bvh.getTime()) << "s / " << round(bvh.getDuration()) << "s" << endl
        << "Position: " << bvh.getPosition() << endl
        << "Height: " << height << endl;
        ofDrawBitmapString(topText.str(), 10, 20);
        
        stringstream bottomText;
        bottomText
        << "f: toggle fullscreen" << endl
        << "space: toggle playback" << endl
        << "left: rewind 1 second" << endl
        << "right: fast forward 1 second" << endl
        << round(ofGetFrameRate()) << "fps";
        int bottomHeight = ofSplitString(bottomText.str(), "\n").size();
        ofDrawBitmapString(bottomText.str(), 10, h-((bottomHeight+1)*12));
    }
    void keyPressed(int key) {
        if (key == ' ') {
            if (bvh.isPlaying()) {
                bvh.stop();
            } else {
                bvh.play();
            }
        }
        if (key == '\t') {
            loadNextFile();
        }
        if (key == 'f') {
            ofToggleFullscreen();
        }
        if (key == OF_KEY_LEFT) {
            bvh.setTime(bvh.getTime() - 1);
            resetSkippedFrames();
        }
        if (key == OF_KEY_RIGHT) {
            bvh.setTime(bvh.getTime() + 1);
            resetSkippedFrames();
        }
    }
};

int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
