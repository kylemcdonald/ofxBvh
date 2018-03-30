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
    string filename = "";
    
    void setup() {
        ofBackground(0);
        ofSetVerticalSync(true);
    }
    void dragged(ofDragInfo& drag) {
        if (drag.files.empty()) return;
        filename = drag.files[0];
        if (ofFile(filename).getExtension() != "bvh") return;
        bvh = ofxBvh(filename);
        bvh.update();
        height = getHeight(bvh);
        bvh.play();
        bvh.setLoop(true);
    }
    void update() {
        if (!bvh.ready()) return;
        if (!bvh.isPlaying()) {
            bvh.setPosition((float) mouseX / ofGetWidth());
        }
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
        stringstream topText;
        topText
        << filename << endl
        << "Frame: " << bvh.getFrame() << "/" << bvh.getNumFrames() << " @ " << bvh.getFrameRate() << "fps" << endl
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
        if (key == '\t') {
            if (bvh.isPlaying()) {
                bvh.stop();
            } else {
                bvh.play();
            }
        }
        if (key == 'f') {
            ofToggleFullscreen();
        }
        if (key == OF_KEY_LEFT) {
            bvh.setTime(bvh.getTime() - 1);
        }
        if (key == OF_KEY_RIGHT) {
            bvh.setTime(bvh.getTime() + 1);
        }
    }
};

int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
