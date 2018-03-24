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

Motion data in a BVH files is a list of real numbers (`double` precision). ofxBvh converts this into local and global matrices like this. If you manually modify the `ofxBvhJoint::localMat`, you should call `ofxBvh::read()` then `ofxBvh::update()` to keep everything in sync. If you manually modify `ofxBvhJoint::raw` data, you should call `ofxBvh::readJointsRaw()` then `ofxBvh::update()` to keep everything in sync.

Converting from a file all the way to matrices happens like this:

- `ofxBvh::load()`: read from a file to `vector<vector<double>> ofxBvh::motion` 
- `ofxBvh::update()` calls two methods internally:
	- `ofxBvh::updateJointsRaw()` calls `ofxBvhJoint::updateRaw()` on each joint, which splits `vector<vector<double>> ofxBvh::motion` into each `vector<double> ofxBvhJoint::raw`
	- `ofxBvh::updateJointsMatrix()` calls `ofxBvhJoint::updateMatrix()` on each joint, which converts `vector<double> ofxBvhJoint::raw` into `ofxBvhJoint::localMat` and `ofxBvhJoint::globalMat`

We can also convert the other direction, from matrices to a file:

- `ofxBvh::read()` calls two methods internally:
	- `ofxBvh::readJointsMatrix()` calls `ofxBvhJoint::readMatrix()` on each joint, which converts `ofxBvhJoint::localMat` into `vector<double> ofxBvhJoint::raw`
	- `ofxBvh::readJointsRaw()` calls `ofxBvhJoint::readRaw()` on each joint, which joins each `vector<double> ofxBvhJoint::raw` into `vector<vector<double>> ofxBvh::motion`
- `ofxBvh::write()`: writes from `vector<vector<double>> ofxBvh::motion` to a file

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