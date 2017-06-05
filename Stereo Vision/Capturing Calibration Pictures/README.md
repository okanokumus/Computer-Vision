Most important part of the stereo vision is to calibrate the cameras for that be sure about the stereo camera's rows are aligned or not.
* Fix the one camera first after that mount the other camera. 
* Select a pattern. (Checkerboard Pattern in this repo)
* Mount the pattern onto a rigid flat surface
* Make sure that you hold the checkerboard horizontally (more checkers horizontally than vertically).
* In order to get a good calibration you will need to move the checkerboard around in the camera frame such that:
    -> the checkerboard is detected at the left and right edges of the field of view (X calibration)
    -> the checkerboard is detected at the top and bottom edges of the field of view (Y calibration)
    -> the checkerboard is detected at various angles to the camera ("Skew")
    -> the checkerboard fills the entire field of view (Size calibration)
    -> checkerboard tilted to the left, right, top and bottom (X,Y, and Size calibration)
* Take photo with pattern at least 15 for each camera in different poses. I have used 40 images for both channels.
* Discard the pairs that have high reprojection errors. And mean error is acceptable between 0 - 1.


References: 
1. http://wiki.ros.org/camera_calibration/Tutorials/StereoCalibration
2. https://boofcv.org/index.php?title=Tutorial_Camera_Calibration
