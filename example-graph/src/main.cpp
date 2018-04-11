#include "ofMain.h"
#include "ofxBvh.h"

class ofApp : public ofBaseApp {
public:
    ofxBvh bvh;
    ofEasyCam cam;
    bool showLocal = true;
    bool normalized = false;
    
    vector<string> jointRotationNames;
    vector<vector<ofMesh>> jointRotationMeshes;
    vector<vector<ofMesh>> jointRotationMeshesNormalized;
    
    void setup() {
        ofBackground(0);
    }
    void loadBvh(string fn) {
        ofJson settings = ofLoadJson("settings.json");
        bool useCentering = settings["useCentering"];
        string visualization = settings["visualization"];
        
        jointRotationNames.clear();
        jointRotationMeshes.clear();
        jointRotationMeshesNormalized.clear();
        
        ofLog() << "Loading file...";
        
        bvh = ofxBvh(fn);
        bvh.play();
        
        ofLog() << "Collecting all rotations...";
        
        // collect all joint rotation data
        int n = bvh.getNumFrames();
        int m = bvh.getJoints().size();
        vector<vector<glm::quat>> rotationData(m, vector<glm::quat>(n));
        vector<vector<glm::vec3>> rawRotationData(m, vector<glm::vec3>(n));
        for(int i = 0; i < n; i++) {
            bvh.setFrame(i);
            bvh.update();
            int j = 0;
            for(auto joint : bvh.getJoints()) {
                glm::quat q(joint->localMat);
                rotationData[j][i] = glm::normalize(q);
                rawRotationData[j][i] = joint->getRotationRaw();
                j++;
            }
        }
        
        // align orientation of all quats
        for(int j = 0; j < m; j++) {
            for(int i = 1; i < n; i++) {
                glm::quat& q = rotationData[j][i];
                glm::quat& pq = rotationData[j][i-1];
                float current = 0, inverted = 0;
                for(int k = 0; k < 4; k++) {
                    current += abs(pq[k] - q[k]);
                    inverted += abs(pq[k] + q[k]);
                }
                if(inverted < current) {
                    q *= -1;
                }
            }
        }
        
        // "center" all quats to initial orientation
        if(useCentering) {
//            cout << "initial centers:" << endl;
            for(int j = 0; j < m; j++) {
                glm::quat initial = rotationData[j][0];
//                cout << initial << endl;
                for(int i = 0; i < n; i++) {
                    rotationData[j][i] *= glm::inverse(initial);
                }
            }
        }
        
        float minRange = 1e-10;
        int components = visualization == "quat" ? 4 : 3;
        for(int j = 0; j < m; j++) {
            vector<ofMesh> meshes, meshesNormalized;
            int reasonable = 0;
            const ofxBvhJoint* joint = bvh.getJoints()[j];
            string name = joint->name;
            if(name == "Solving") continue;
//            glm::quat& initial = rotationData[0][j];
            for(int k = 0; k < components; k++) {
                ofMesh mesh, meshNormalized;
                mesh.setMode(OF_PRIMITIVE_LINE_STRIP);
                meshNormalized.setMode(OF_PRIMITIVE_LINE_STRIP);
                float minValue = 0, maxValue = 0;
                float minAngle = 0, maxAngle = 0;
                vector<float> values;
                for(int i = 0; i < n; i++) {
                    glm::quat q = rotationData[j][i];
                    
                    float x;
                    if (visualization == "quat") {
                        // raw quaternion approach
                        x = q[k];
                        x = (x / 2) + 0.5;
                    } else if (visualization == "raw") {
                        // use raw/native euler
                        x = rawRotationData[j][i][k];
                        x = (x / 360) + 0.5; // from -180/+180 to 0/1
                    } else if (visualization == "euler") {
                        // generate euler from quat
                        // has the nice property of always looping around
                        // bad property of gimbal lock
                        x = glm::eulerAngles(q)[k]; // should this be different for different orders?
                        x = (x / TWO_PI) + 0.5; // from -PI/+PI to 0/1
                    }
                    
                    if(i == 0 || x < minValue) minValue = x;
                    if(i == 0 || x > maxValue) maxValue = x;
                    
                    values.push_back(x);
                }
                float range = maxValue - minValue;
                if(range > minRange) reasonable++;
                
                for(int i = 0; i < n; i++) {
                    float x = values[i];
                    mesh.addVertex(ofVec3f((float) i / n, x));
                    x = (x - minValue) / (maxValue - minValue);
                    meshNormalized.addVertex(ofVec3f((float) i / n, x));
                }
                meshes.emplace_back(mesh);
                meshesNormalized.emplace_back(meshNormalized);
            }
            if(reasonable > 0) { // change to > to exclude some
                jointRotationMeshes.emplace_back(meshes);
                jointRotationMeshesNormalized.emplace_back(meshesNormalized);
                jointRotationNames.emplace_back(name);
            }
        }
    }
    void dragged(ofDragInfo& drag) {
        if (drag.files.empty()) return;
        for (auto fn : drag.files) {
            if (ofFile(fn).getExtension() == "bvh") {
                loadBvh(fn);
                break;
            }
        }
    }
    void update() {
        if (!bvh.ready()) return;
        if (!bvh.isPlaying()) {
            int index = ((float) mouseX / ofGetWidth()) * bvh.getNumFrames();
            bvh.setFrame(index);
        }
        bvh.update();
    }
    void draw() {
        if (!bvh.ready()) {
            ofDrawBitmapString("Drop bvh file.", ofGetWidth()/2, ofGetHeight()/2);
            return;
        }
        
        ofSetColor(255);
        
        int index = bvh.getFrame();
        float x = (ofGetWidth() * index) / bvh.getNumFrames();
        ofDrawLine(x, 0, x, ofGetHeight());
        ofDrawBitmapString(ofToString(index), ofGetWidth() / 2, ofGetHeight() - 20);
        
        cam.begin();
        bvh.draw();
        cam.end();
        
        float nodeScale = 50;
        ofCamera view = cam;
        int n = jointRotationMeshes.size();
        float height = ofGetHeight() / n;
        for(int i = 0; i < n; i++) {
            string name = jointRotationNames[i];
            
            const vector<ofMesh>& mesh = normalized ?
                jointRotationMeshesNormalized[i] :
                jointRotationMeshes[i];
            drawRotationGraph(mesh,
                              ofRectangle(0, i * height, ofGetWidth(), height),
                              name);
            const ofxBvhJoint* joint = bvh.getJoint(name);
            
            ofNode node;
            node.setOrientation(showLocal ?
                joint->localMat :
                joint->globalMat);
            view.begin(ofRectangle(x + 10, i * height, height, height));
            ofScale(nodeScale);
            node.draw();
            view.end();
        }
    }
    void drawRotationGraph(const vector<ofMesh>& meshes, ofRectangle viewport, string name="") {
        ofPushStyle();
        ofPushMatrix();
        ofTranslate(viewport.x, viewport.y);
        ofSetColor(255);
        ofDrawBitmapString(name, 10, 20);
        ofScale(viewport.width, viewport.height);
        for(int i = 0; i < meshes.size(); i++) {
            ofSetColor(vector<ofColor>{ofColor::cyan, ofColor::magenta, ofColor::yellow, ofColor::white}[i]);
            meshes[i].draw();
        }
        ofPopMatrix();
        ofPopStyle();
    }
    void keyPressed(int key) {
        if(key == '\t') {
            if(bvh.isPlaying()) {
                bvh.stop();
            } else {
                bvh.play();
            }
        }
        if(key == 'f') {
            ofToggleFullscreen();
        }
        if(key == 'l') {
            showLocal = !showLocal;
        }
        if(key == 'n') {
            normalized = !normalized;
        }
    }
};

int main() {
    ofSetupOpenGL(1024, 1024, OF_WINDOW);
    ofRunApp(new ofApp());
}
