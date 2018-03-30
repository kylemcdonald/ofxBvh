#include "ofMain.h"
#include "ofxBvh.h"

class ofApp : public ofBaseApp {
public:
    ofxBvh bvh;
    ofEasyCam cam;
    
    void setup() {
        ofSetVerticalSync(true);
        ofBackground(0);
        ofNoFill();
        
        bvh = ofxBvh("vicon.bvh");
        bvh.play();
        
        // crop files and check the results
//        bvh.back().cropToFrame(1200);
//        bvh.back().cropToTime(2, 8);
        
        // save files
//        bvh.save("vicon-copy.bvh");
    }
    void update() {
        bvh.update();
        
        // test that read + update work correctly
//        b.read();
//        b.update();
    }
    void draw() {
        cam.begin();
        ofScale(2,2,2);
        ofTranslate(0,-75,0);
        
        bvh.draw();
    
        // select joints by index
        int n = bvh.getJoints().size();
        for(int i = 0; i < n; i+=5) {
            ofPushMatrix();
            ofMultMatrix(bvh.getJoints()[i]->globalMat);
            ofDrawBitmapString(ofToString(i), 0, 0);
            ofPopMatrix();
        }
        
        // select joints by name
        ofxBvhJoint* head = bvh.getJoint("Head");
        if (head != nullptr) {
            head = head->getChildren()[0].get();
            ofPushMatrix();
            ofMultMatrix(head->globalMat);
            ofDrawBitmapString("Head", 10, 0);
            ofPopMatrix();
        }
        
        cam.end();
        
        ofSetColor(255);
        ofDrawBitmapString(ofToString(round(ofGetFrameRate())), 10, 20);
    }
};

int main() {
    ofSetupOpenGL(1280, 720, OF_WINDOW);
    ofRunApp(new ofApp());
}
