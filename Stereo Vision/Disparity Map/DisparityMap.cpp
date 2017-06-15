// DisparityMap.cpp : Defines the entry point for the console application.
//
// There is inverse relation depth and disparity 
// distance = (baseline * focal length) / disparity

#include "stdafx.h"
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include "opencv2/stereo/stereo.hpp"

using namespace std;
using namespace cv;

struct container{

	Mat img; // colored image
	Mat gray; // grayscale
	Mat imgU; // undistorted 
	Mat mapx, mapy; // first and second output map (will be used in remap function)

}LeftCam, RightCam;// A structure for image containers
// creating image container

FileStorage file;

Mat disp, disp8;
Mat P1, P2, CM1, CM2, D1, D2, R1, R2;
Mat R, T, E, F, Q;


void open_calib_file();
int main(){
	Rect roi1, roi2;
	open_calib_file();
	// loading stereo pairs
	LeftCam.img = imread("...", 1); // addr of the left image
	RightCam.img = imread("...", 1); // addr of the right image
	if (!LeftCam.img.data || !RightCam.img.data) {
		cout << "images are not loaded:";
		return -1;
	}

	stereoRectify(CM1, D1, CM2, D2, LeftCam.img.size(), R, T, R1, R2, P1, P2, Q, CALIB_ZERO_DISPARITY, -1, LeftCam.img.size(), &roi1, &roi2);

	initUndistortRectifyMap(CM1, D1, R1, P1, LeftCam.img.size(), CV_32FC1, LeftCam.mapx, LeftCam.mapy);
	initUndistortRectifyMap(CM2, D2, R2, P2, RightCam.img.size(), CV_32FC1, RightCam.mapx, RightCam.mapy);
	remap(LeftCam.img, LeftCam.imgU, LeftCam.mapx, LeftCam.mapy, INTER_LINEAR, BORDER_CONSTANT, Scalar());
	remap(RightCam.img, RightCam.imgU, RightCam.mapx, RightCam.mapy, INTER_LINEAR, BORDER_CONSTANT, Scalar());

	// convert the colored image to grayscale 
	cvtColor(LeftCam.imgU, LeftCam.gray, CV_RGB2GRAY);
	cvtColor(RightCam.imgU, RightCam.gray, CV_RGB2GRAY);
	// create StereoBM object 
	// computing stereo correspondence using the block matching algorithm
	Ptr< StereoBM > sbm;
	sbm = StereoBM::create(16, 9);
	sbm->setROI1(roi1);
	sbm->setROI2(roi2);
	sbm->setNumDisparities(112); // it can divided by 16
	sbm->setPreFilterSize(5);
	sbm->setPreFilterCap(31);
	sbm->setMinDisparity(0);
	sbm->setTextureThreshold(507);
	sbm->setUniquenessRatio(10);
	sbm->setSpeckleWindowSize(100);
	sbm->setSpeckleRange(32);
	sbm->setDisp12MaxDiff(1);
	sbm->setBlockSize(21); //SADWindowSize 
	// compute the disparity
	sbm->compute(LeftCam.gray, RightCam.gray, disp);

	
	imshow("left image", LeftCam.img);
	imshow("right image", RightCam.img);
	
	// normalized the disp matrix for showing
	normalize(disp, disp8, 0, 255, CV_MINMAX, CV_8U);
	imshow("disparity image", disp8);
	
	waitKey(0);
	return 0;
}

void open_calib_file() {
	file.open("..../mystereocalib.yml", FileStorage::READ); // addr of calib  file
	// to get projection matrices and some other matrices for required undistortion
	file["CM1"] >> CM1;
	file["CM2"] >> CM2;
	file["D1"] >> D1;
	file["D2"] >> D2;
	file["R"] >> R;
	file["T"] >> T;
}


