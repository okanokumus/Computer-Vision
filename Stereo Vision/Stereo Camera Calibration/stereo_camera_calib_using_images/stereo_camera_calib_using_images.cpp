// stereo_camera_calib_using_images.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\calib3d\calib3d.hpp>

using namespace std;
using namespace cv;

vector< vector< Point3f > > object_points;
vector< vector< Point2f > > imagePoints1, imagePoints2;
vector< Point2f > corners1, corners2;
vector< vector< Point2f > > left_img_points, right_img_points;
vector<Point3f> obj;
Mat img1, gray1, img2, gray2; // images and garyscale containers of them

int main()
{
	int no_images = 16; // number of images
	char* extension = ".png"; // extension of the images
	char* address1 = ".../cam1/"; // address of the images of the left camera
	char* address2 = ".../cam2/"; // address of the images of the right camera
	int success = 0, k = 0;
	int board_height = 9; // the number of internal corners vertically
	int board_width = 6; // the number of internal corners horizontally
	int board_n = board_width*board_height; // number of squares
	double square_size = 35.00;
	Size board_size = Size(6, 9);
	
	for (int j = 0; j<board_n; j++){
		// This creates a list of coordinates (0,0,0), (0,1,0), (0,2,0)...(1,4,0)... so on. 
		// Each corresponds to a particular vertex.
		obj.push_back(Point3f(j / board_width, j%board_width, 0.0f));
	}

	for (int x = 1; x < no_images; x++) {
		// read the both channels images
		char filename1[200], filename2[200];
		sprintf_s(filename1, "%scam1_%d%s", address1, x, extension);// name and extension of the iamges for the left camera
		sprintf_s(filename2, "%scam2_%d%s", address2, x, extension);// name and extension of the iamges for the right camera
		img1 = imread(filename1, CV_LOAD_IMAGE_COLOR);
		img2 = imread(filename2, CV_LOAD_IMAGE_COLOR);
		// convert all images to the grayscale
		cvtColor(img1, gray1, CV_RGB2GRAY);
		cvtColor(img2, gray2, CV_RGB2GRAY);
		// end of the reading

		//to find all the checkerboard corners
		bool found1 = false, found2 = false;
		found1 = findChessboardCorners(gray1, board_size, corners1,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		found2 = findChessboardCorners(gray2, board_size, corners2,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);

		if (found1) {
			cornerSubPix(gray1, corners1, Size(5, 5), Size(-1, -1),
				TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			drawChessboardCorners(img1, board_size, corners1, found1);
		}
		if (found2) {
			cornerSubPix(gray2, corners2, Size(5, 5), Size(-1, -1),
				TermCriteria(CV_TERMCRIT_EPS | CV_TERMCRIT_ITER, 30, 0.1));
			drawChessboardCorners(img2, board_size, corners2, found2);
		}
		imshow("image1", img1);
		imshow("image2", img2);
		k = waitKey(10);
		if (found1 && found2)	k = waitKey(0);
		if (k == 27)break;

		if (found1 && found2)
		{

			imagePoints1.push_back(corners1);
			imagePoints2.push_back(corners2);
			object_points.push_back(obj);
			printf("Corners stored\n");
			success++;

			if (success >= no_images)break;
		}
	}
	destroyAllWindows();
	cout << imagePoints1.size() << endl;
	cout << object_points.size() << endl;
	printf("Starting Calibration\n");
	Mat CM1 = Mat(3, 3, CV_64FC1);
	Mat CM2 = Mat(3, 3, CV_64FC1);
	Mat D1, D2;
	Mat R, T, E, F;

	double rms_stereo = stereoCalibrate(object_points, imagePoints1, imagePoints2,
		CM1, D1, CM2, D2, img1.size(), R, T, E, F,
		(TermCriteria::MAX_ITER + TermCriteria::EPS, 100, 1e-5));
	cout << "rms error of the stereo camera calibraiton : " << rms_stereo << endl;
	FileStorage fs1("mystereocalib.yml", FileStorage::WRITE); // save parameters as a .yml file into the same folder
	fs1 << "CM1" << CM1;
	fs1 << "CM2" << CM2;
	fs1 << "D1" << D1;
	fs1 << "D2" << D2;
	fs1 << "R" << R;
	fs1 << "T" << T;
	fs1 << "E" << E;
	fs1 << "F" << F;
	fs1 << "Error" << rms_stereo; // rms error for stereo camera calibration
	cout << "Done Calibration\n";
	/*	CM1 - Camera Matrix of first camera.
	CM2 - Camera Matrix of second camera.
	D1 - Distortion coeff matrix of first camera.
	D2 - Distortion coeff matrix of second camera.
	R - Rotation Matrix between first and second camera coordinate systems.
	T - Translation vector between the coordinate systems of the cameras.
	E - Essential matrix.
	F - Fundamental matrix.
	*/
	cout << "Starting Rectification\n";
	Mat R1, R2, P1, P2, Q;
	stereoRectify(CM1, D1, CM2, D2, img1.size(), R, T, R1, R2, P1, P2, Q, 1024);
	fs1 << "R1" << R1;
	fs1 << "R2" << R2;
	fs1 << "P1" << P1;
	fs1 << "P2" << P2;
	fs1 << "Q" << Q;
	cout << "Done Rectification\n";
	/*	R1 - 3x3 rectification transform(rotation matrix) for the first camera.
	R2 - 3x3 rectification transform(rotation matrix) for the second camera.
	P1 - 3x4 projection matrix in the new (rectified) coordinate systems for the first camera.
	P2 - 3x4 projection matrix in the new (rectified) coordinate systems for the second camera.
	Q – 4x4 disparity - to - depth mapping matrix.
	*/
	return 0;
}
