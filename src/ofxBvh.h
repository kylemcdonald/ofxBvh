#pragma once

#include "ofVectorMath.h"

class ofxBvhJoint {
    friend class ofxBvh;
    
private:
    int channels = 0;
    std::string rotationOrder;
    
    void dumpHierarchy(std::ostream& output, std::string tabs="");
    void updateHierarchy(std::vector<double>::const_iterator& frame, glm::mat4 global=glm::mat4());
    void readHierarchy(std::vector<double>::iterator& frame);
    void drawHierarchy(bool drawNames=false);
    unsigned int countJoints();
    ofxBvhJoint* getJoint(int target, int& counter);
    ofxBvhJoint* getJoint(const std::string& target);
    
public:
    std::string name;
    glm::vec3 offset;
    glm::mat4 localMat, globalMat;
    ofxBvhJoint* parent = nullptr;
    std::vector<std::shared_ptr<ofxBvhJoint>> children;
    
    inline bool isSite() const { return children.empty(); }
    inline bool isRoot() const { return !parent; }
};

class ofxBvh {
private:
    std::shared_ptr<ofxBvhJoint> root;
    double frameTime = 0;
    unsigned int numJoints = 0;
    std::vector<std::vector<double>> motion;
    
    float playRate = 1;
    float startTime = 0;
    unsigned int startFrame = 0;
    unsigned int frameNumber = 0;
    bool playing = false;
    bool loop = true;
    bool frameNew = false;
    
    static void dumpMotion(std::ostream& output, float frameTime, const std::vector<std::vector<double>>& motion);
    
public:
    
    void load(std::string filename);
    void save(std::string filename) const;
    void update(); // update joints using motion data
    void read(); // update motion data using joints
    bool isFrameNew() const;
    void draw(bool drawNames=false) const;
    std::string info() const;
    
    unsigned int getNumJoints() const;
    ofxBvhJoint* getJoint(int index);
    ofxBvhJoint* getJoint(const std::string& name);
    
    void play();
    void stop();
    void setRate(float playRate);
    float getRate() const;
    void togglePlaying();
    bool isPlaying() const;
    
    void setLoop(bool loop);
    bool isLoop() const;
    
    float getDuration() const; // in seconds
    unsigned int getNumFrames() const; // total frame count
    float getFrameDuration() const; // in seconds
    float getFrameRate() const; // in frames per second
    unsigned int getFrame() const;
    float getTime() const; // current time in seconds
    float getPosition() const; // current position 0-1
    
    void setFrame(unsigned int frameNumber);
    void setTime(float seconds); // set time in seconds
    void setPosition(float ratio); // set position 0-1
    
    // cropping will cause playback to stop
    void cropToFrame(unsigned int beginFrameNumber, unsigned int endFrameNumber);
    void cropToTime(float beginSeconds, float endSeconds);
    void cropToPosition(float beginRatio, float endRatio);
};
