# ofxBvh

This addon was initially distributed as an example with the [Perfume Global Project](https://github.com/perfume-dev/example-openFrameworks/blob/master/ofxBvh/src/ofxBvh.h) but has since been rewritten completely from scratch. The new version maintains some of the old functionality but also adds some important features.

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