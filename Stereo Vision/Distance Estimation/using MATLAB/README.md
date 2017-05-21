Find distance of a person through the cameras. Two cameras are used to camera calibration and 'Stereo Camera Calibrator' tool of MATLAB
are used for prework. For camera calibration 6x9 checkerboard pattern is used(it is available in this repo). After calibrarion matrices 
are obtained which name is 'StereoParams6x9', finding distance process is started.
For one detected face in each frame, code works fine but more than one face and in the case of unequal detected faces, use 'compare_y_axes' to solve correspondence problem but it not a unique solution(bad actullay)
