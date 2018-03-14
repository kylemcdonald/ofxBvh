#pragma once

#include "ofVectorMath.h"
#include <map>

class ofxBvh;

class ofxBvhJoint
{
    friend class ofxBvh;
    
public:
    
    enum CHANNEL
    {
        X_ROTATION, Y_ROTATION, Z_ROTATION,
        X_POSITION, Y_POSITION, Z_POSITION
    };
    
    ofxBvhJoint(std::string name, ofxBvhJoint *parent) : name(name),  parent(parent) {}
    
    inline const std::string& getName() const { return name; }
    inline const ofVec3f& getOffset() const { return offset; }
    
    inline const ofMatrix4x4& getMatrix() const { return matrix; }
    inline const ofMatrix4x4& getGlobalMatrix() const { return global_matrix; }
    
    inline ofVec3f getPosition() const { return global_matrix.getTranslation(); }
    inline ofQuaternion getRotate() const { return global_matrix.getRotate(); }
    
    inline ofxBvhJoint* getParent() const { return parent; }
    inline const std::vector<ofxBvhJoint*>& getChildren() const { return children; }
    
    inline bool isSite() const { return children.empty(); }
    inline bool isRoot() const { return !parent; }
    
    inline ofxBvh* getBvh() const { return bvh; }
    
protected:
    
    std::string name;
    ofVec3f initial_offset;
    ofVec3f offset;
    
    ofMatrix4x4 matrix;
    ofMatrix4x4 global_matrix;
    
    ofxBvh* bvh;
    
    std::vector<ofxBvhJoint*> children;
    ofxBvhJoint* parent;
    
    std::vector<CHANNEL> channel_type;
};

class ofxBvh
{
public:
    
    ofxBvh() : root(NULL), total_channels(0), rate(1), loop(false),
    playing(false), play_head(0), need_update(false) {}
    
    virtual ~ofxBvh();
    
    void load(std::string path);
    void unload();
    
    void update();
    void draw();
    
    bool isFrameNew() const;
    
    void play();
    void stop();
    bool isPlaying() const;
    
    void setLoop(bool yn);
    bool isLoop() const;
    
    void setRate(float rate);
    
    void setFrame(int index);
    int getFrame() const;
    int getNumFrames() const;
    
    void setPosition(float pos);
    float getPosition() const;
    
    float getDuration() const;
    
    int getNumJoints() const { return joints.size(); }
    const ofxBvhJoint* getJoint(int index) const;
    const ofxBvhJoint* getJoint(const std::string &name) const;
    
protected:
    
    typedef std::vector<float> FrameData;
    
    int total_channels;
    
    ofxBvhJoint* root;
    std::vector<ofxBvhJoint*> joints;
    std::map<std::string, ofxBvhJoint*> jointMap;
    
    std::vector<FrameData> frames;
    FrameData currentFrame;
    
    int num_frames;
    float frame_time;
    
    float rate;
    
    bool playing;
    float play_head;
    
    bool loop;
    bool need_update;
    bool frame_new;
    
    void parseHierarchy(const std::string& data);
    ofxBvhJoint* parseJoint(int& index, std::vector<std::string> &tokens, ofxBvhJoint *parent);
    void updateJoint(int& index, const FrameData& frame_data, ofxBvhJoint *joint);
    
    void parseMotion(const std::string& data);
    
};
