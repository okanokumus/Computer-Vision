// Stereo_Calibration.cpp : Defines the entry point for the console application.
//

// firstly takes pictures and the if it can be calibrated, it is accepted. Process is valid for both cameras

// References:
// http://docs.opencv.org/2.4/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html
// http://sourishghosh.com/2016/stereo-calibration-cpp-opencv/
// http://www.jayrambhia.com/blog/stereo-calibration



#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <vector>
#include "opencv2/core/core.hpp"
#include "opencv2/core/types.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/calib3d/calib3d.hpp>

using namespace cv;
using namespace std;

int main()
{
	char filename1[200], filename2[200]; // write images 
	char* extension = ".png"; // extension of images
	char* address1 = ".../cam1/" ;
	char* address2 = ".../cam2/" ;

	int numBoards = 15; // number of different poses (10-20 poses are sufficient)
	int board_w = 6; // number of horizontal corners
	int board_h = 9; // number of vertical corners

	Size board_sz = Size(board_w, board_h);
	int board_n = board_w*board_h;

	vector<vector<Point3f> > object_points; //  the actual 3D coordinate of those checkerboard points.
	vector<vector<Point2f> > imagePoints1, imagePoints2; // checkerboard corner coordinates in the image
	vector<Point2f> corners1, corners2; // detected corners
	vector<Point3f> obj;

	for (int j = 0; j<board_n; j++)
	{
		obj.push_back(Point3f(j / board_w, j%board_w, 0.0f));
	}

	// images and garyscale containers of them
	Mat img1, img2, gray1, gray2; 
	
	VideoCapture cap1(1); // first camera
	VideoCapture cap2(2); // second camera

	int success = 0, k = 0;
	bool found1 = false, found2 = false;

	while (success < numBoards)
	{
		// take images from two cameras
		cap1 >> img1; 
		cap2 >> img2;

		// convert images to grayscale 
		cvtColor(img1, gray1, CV_BGR2GRAY);
		cvtColor(img2, gray2, CV_BGR2GRAY);

		// to find all the checkerboard corners
		found1 = findChessboardCorners(img1, board_sz, corners1,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		found2 = findChessboardCorners(img2, board_sz, corners2, 
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

		// returns bool if found or not
		if (found1)
		{
			// refine the corner locations
			cornerSubPix(gray1, corners1, Size(11, 11), Size(-1, -1),
				TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			// visualize the checkerboard corners found
			drawChessboardCorners(gray1, board_sz, corners1, found1); 
			sprintf_s(filename1, "%scam1_%d%s",address1, success,extension);
			imwrite(filename1, img1);
			
		}

		if (found2)
		{
			// refine the corner locations
			cornerSubPix(gray2, corners2, Size(11, 11), Size(-1, -1),
				TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			// visualize the checkerboard corners found
			drawChessboardCorners(gray2, board_sz, corners2, found2); 
			sprintf_s(filename2, "%scam2_%d%s", address2, success, extension);
			imwrite(filename2, img2);
		}

		imshow("image1", gray1);
		imshow("image2", gray2);

		k = waitKey(10);
		
		if (found1 && found2){k = waitKey(0);}
		if (k == 27){break;}

		if (k == ' ' && found1 != 0 && found2 != 0){
			imagePoints1.push_back(corners1);
			imagePoints2.push_back(corners2);
			object_points.push_back(obj);
			cout <<"Corners stored : ( " << numBoards-success-1 << " remaings ) \n" ;
			success++;

			if (success >= numBoards){break;}
		}
	}

	destroyAllWindows();
	cout << "Starting Calibration\n";
	Mat CM1 = Mat(3, 3, CV_64FC1);
	Mat CM2 = Mat(3, 3, CV_64FC1);
	Mat D1, D2;
	Mat R, T, E, F;

	stereoCalibrate(object_points, imagePoints1, imagePoints2,
		CM1, D1, CM2, D2, img1.size(), R, T, E, F,
		(TermCriteria::MAX_ITER + TermCriteria::EPS, 100, 1e-5));

	FileStorage fs1("mystereocalib.yml", FileStorage::WRITE);
	fs1 << "CM1" << CM1;
	fs1 << "CM2" << CM2;
	fs1 << "D1" << D1;
	fs1 << "D2" << D2;
	fs1 << "R" << R;
	fs1 << "T" << T;
	fs1 << "E" << E;
	fs1 << "F" << F;
	/*	CM1 - Camera Matrix of first camera.
	CM2 - Camera Matrix of second camera.
	D1 - Distortion coeff matrix of first camera.
	D2 - Distortion coeff matrix of second camera.
	R - Rotation Matrix between first and second camera coordinate systems.
	T - Translation vector between the coordinate systems of the cameras.
	E - Essential matrix.
	F - Fundamental matrix.
	*/
	cout << "Done Calibration\n";

	cout << "Starting Rectification\n";

	Mat R1, R2, P1, P2, Q;
	stereoRectify(CM1, D1, CM2, D2, img1.size(), R, T, R1, R2, P1, P2, Q);
	fs1 << "R1" << R1;
	fs1 << "R2" << R2;
	fs1 << "P1" << P1;
	fs1 << "P2" << P2;
	fs1 << "Q" << Q;
	/*	R1 - 3x3 rectification transform(rotation matrix) for the first camera.
	R2 - 3x3 rectification transform(rotation matrix) for the second camera.
	P1 - 3x4 projection matrix in the new (rectified) coordinate systems for the first camera.
	P2 - 3x4 projection matrix in the new (rectified) coordinate systems for the second camera.
	Q – 4x4 disparity - to - depth mapping matrix.
	*/
	cout << "Done Rectification\n";
	/*
	printf("Applying Undistort\n");

	Mat map1x, map1y, map2x, map2y;
	Mat imgU1, imgU2;

	initUndistortRectifyMap(CM1, D1, R1, P1, img1.size(), CV_32FC1, map1x, map1y);
	initUndistortRectifyMap(CM2, D2, R2, P2, img2.size(), CV_32FC1, map2x, map2y);

	printf("Undistort complete\n");

	while (1)
	{
	cap1 >> img1;
	cap2 >> img2;

	remap(img1, imgU1, map1x, map1y, INTER_LINEAR, BORDER_CONSTANT, Scalar());
	remap(img2, imgU2, map2x, map2y, INTER_LINEAR, BORDER_CONSTANT, Scalar());

	imshow("image1", imgU1);
	imshow("image2", imgU2);

	k = waitKey(5);

	if (k == 27)
	{
	break;
	}
	}
	*/
	cap1.release();
	cap2.release();

	return(0);
}
