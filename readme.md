# ofxBvh

This addon was began as an example in the [Perfume Global Project](https://github.com/perfume-dev/example-openFrameworks/blob/master/ofxBvh/src/ofxBvh.h) written by [Satoru Higa](https://github.com/satoruhiga) but has since been rewritten from scratch. The new version maintains some of the old functionality but also adds some important features.

 Good:
 - Faster file loading than before.
 - Writing from modified skeletons back to motion data.
 - Saving BVH files.
 - Supports multiple (but not all) BVH formats.
 - Playback doesn't accumulate error.
 
 Bad:
 - Doesn't report all file reading errors.
 - Playing in reverse past the beginning is undefined, but will not crash.
 - Only loads one ROOT hierarchy per file (same as original addon).

## Porting old ofxBvh code

`ofxBvh::unload()` is not needed anymore and has been removed.

Some variables that are safe to read/write from have been made public, and methods for accessing them have been removed:

```c++
ofxBvh::getNumJoints(); // ofxBvh::getJoints().size()
ofxBvh::getJoint(int i); // ofxBvh::getJoints()[i]
ofxBvhJoint::getName(); // ofxBvhJoint::name
ofxBvhJoint::getOffset(); // ofxBvhJoint::offset
ofxBvhJoint::getMatrix(); // ofxBvhJoint::localMat
ofxBvhJoint::getGlobalMatrix(); // ofxBvhJoint::globalMat
ofxBvhJoint::getPositionLocal(); // glm::vec3(ofxBvhJoint::localMat[3])
ofxBvhJoint::getRotationLocal(); // glm::quat(ofxBvhJoint::localMat)
ofxBvhJoint::getPosition(); // glm::vec3(ofxBvhJoint::globalMat[3])
ofxBvhJoint::getRotation(); // glm::quat(ofxBvhJoint::globalMat)
```