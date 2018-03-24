#include "ofMain.h"
#include "ofxBvh.h"

string help = "\
f: toggle fullscreen\n\
space: toggle playback\n\
left: rewind 1 second\n\
right: fast forward 1 second\n\
up: speed up 10%\n\
down: slow down 10%";

class ofApp : public ofBaseApp {
public:
    vector<ofxBvh> bvh;
    ofEasyCam cam;
    
    void setup() {
        for(auto fn : vector<string>{
            "vicon.bvh",
            "perfume.bvh"
        }) {
            bvh.emplace_back();
            bvh.back().load(fn);
            cout << bvh.back().info() << " / " << fn << endl;
            
            // crop files and check the results
//            bvh.back().cropToTime(2, 8);
//            cout << bvh.back().info() << " / " << fn << endl;
        }
        
        // save files
//        bvh[0].save("vicon.bvh");
//        bvh[1].save("perfume.bvh");
        
        ofBackground(0);
        ofNoFill();
    }
    void update() {
        for(auto& b : bvh) {
            b.update();
        }
    }
    void draw() {
        cam.begin();
        ofScale(2,2,2);
        float spacing = 200;
        ofTranslate((bvh.size() - 1) * (-spacing / 2), -75);
        for(auto& b : bvh) {
            b.draw();
            
            // select joints by index
            int n = b.getNumJoints();
            for(int i = 0; i < n; i+=5) {
                ofPushMatrix();
                ofMultMatrix(b.getJoint(i)->globalMat);
                ofDrawBitmapString(ofToString(i), 0, 0);
                ofPopMatrix();
            }
            
            // select joints by name
            ofxBvhJoint* head = b.getJoint("Head");
            if (head != nullptr) {
                head = head->children[0].get();
                ofPushMatrix();
                ofMultMatrix(head->globalMat);
                ofDrawBitmapString("Head", 0, 0);
                ofPopMatrix();
            }
            
            ofTranslate(spacing, 0);
        }
        cam.end();
        
        ofSetColor(255);
        ofDrawBitmapString(ofToString(round(ofGetFrameRate())), 10, 20);
        ofDrawBitmapString(help, 10, 40);
    }
    void keyPressed(int key) {
        if (key == 'f') {
            ofToggleFullscreen();
        }
        if (key == ' ') {
            for(auto& b : bvh) {
                b.togglePlaying();
            }
        }
        if (key == OF_KEY_LEFT) {
            for(auto& b : bvh) {
                b.setTime(b.getTime() - 1);
            }
        }
        if (key == OF_KEY_RIGHT) {
            for(auto& b : bvh) {
                b.setTime(b.getTime() + 1);
            }
        }
        if (key == OF_KEY_UP) {
            for(auto& b : bvh) {
                b.setRate(b.getRate() * 1.1);
            }
        }
        if (key == OF_KEY_DOWN) {
            for(auto& b : bvh) {
                b.setRate(b.getRate() / 1.1);
            }
        }
    }
};

int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
