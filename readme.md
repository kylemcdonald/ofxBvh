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

## Modifying matrices and raw data

Motion data in a BVH files is a list of real numbers (`double` precision). ofxBvh converts this into local and global matrices. If you manually modify the `ofxBvhJoint::localMat`, you should call `ofxBvh::read()` then `ofxBvh::update()` to keep everything in sync. If you manually modify `ofxBvhJoint::raw` data, you should call `ofxBvh::readJointsRaw()` then `ofxBvh::update()` to keep everything in sync. More thoroughly, there are four representations:

1. `.bvh` file
2. `ofxBvh::motion`
3. `ofxBvhJoint::raw`
4. `ofxBvhJoint::localMat`

To convert:

- From 1 to 2: `ofxBvh::load()`
- From 2 to 1: `ofxBvh::write()`
- From 2 to 3 to 4: `ofxBvh::update()`
- From 4 to 3 to 2: `ofxBvh::read()`
- From 2 to 3: `ofxBvh::updateRaw()`
- From 3 to 4: `ofxBvh::updateMatrix()`
- From 4 to 3: `ofxBvh::readMatrix()`
- From 3 to 2: `ofxBvh::readRaw()`

## Porting old ofxBvh code

`ofxBvh::load()` has been replaced by a constructor. Instead of `bvh.load("...")` write `bvh = ofxBvh("...")`.

`ofxBvh::unload()` is not needed anymore and has been removed.

Some variables that are safe to read/write from have been made public, and methods for accessing them have been removed. ofxBvh now returns raw glm objects instead of `ofVec3f` or `ofQuaternion`, but openFrameworks will make the conversion automatically.

```c++
ofxBvh::getNumJoints(); // ofxBvh::getJoints().size()
ofxBvh::getJoint(int i); // ofxBvh::getJoints()[i]
ofxBvhJoint::getName(); // ofxBvhJoint::name
ofxBvhJoint::getOffset(); // ofxBvhJoint::offset
ofxBvhJoint::getMatrix(); // ofxBvhJoint::localMat
ofxBvhJoint::getGlobalMatrix(); // ofxBvhJoint::globalMat
```