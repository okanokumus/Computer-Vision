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
Mat img1, gray1, img2, gray2;

int main()
{
	int no_images = 12; // number of images which will you use for calibration
	char* extension = ".png"; 
	char* address1 = "E:/visual stduio codes/image processing codes/stereo vision/Stereo_Calibration/stereo images/1/";
	char* address2 = "E:/visual stduio codes/image processing codes/stereo vision/Stereo_Calibration/stereo images/2/";
	int success = 0, k = 0;
	int board_height = 9;
	int board_width = 6;

	double square_size = 35.00;

	Size board_size = Size(6, 9);
	int board_n = board_width*board_height;
	for (int j = 0; j<board_n; j++)
	{
		obj.push_back(Point3f(j / board_width, j%board_width, 0.0f));
	}

	for (int x = 0; x < no_images; x++) { // x can be changed according to your image name
		// read the both channels images
		char filename1[200], filename2[200];
		sprintf_s(filename1, "%scam1_%d%s", address1, x, extension);
		sprintf_s(filename2, "%scam2_%d%s", address2, x, extension);
		img1 = imread(filename1, CV_LOAD_IMAGE_COLOR);
		img2 = imread(filename2, CV_LOAD_IMAGE_COLOR);
		// convert all images to the grayscale
		cvtColor(img1, gray1, CV_RGB2GRAY);
		cvtColor(img2, gray2, CV_RGB2GRAY);
		// end of the reading

		//to find all the checkerboard corners
		bool found1 = false, found2 = false;
		found1 = findChessboardCorners(img1, board_size, corners1,
			CV_CALIB_CB_ADAPTIVE_THRESH | CV_CALIB_CB_FILTER_QUADS);
		found2 = findChessboardCorners(img2, board_size, corners2,
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
		if (found1 && found2)
		{
			k = waitKey(0);
		}
		if (k == 27)
		{
			break;
		}*/

		if (found1 && found2)
		{

			imagePoints1.push_back(corners1);
			imagePoints2.push_back(corners2);
			object_points.push_back(obj);
			printf("Corners stored\n");
			success++;

			if (success >= no_images)
			{
				break;
			}
		}
	}
	destroyAllWindows();
	cout << imagePoints1.size() << endl;
	cout << imagePoints2.size() << endl;
	printf("Starting Calibration\n");
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
	cout << "Done Calibration\n";

	cout << "Starting Rectification\n";
	Mat R1, R2, P1, P2, Q;
	stereoRectify(CM1, D1, CM2, D2, img1.size(), R, T, R1, R2, P1, P2, Q);
	fs1 << "R1" << R1;
	fs1 << "R2" << R2;
	fs1 << "P1" << P1;
	fs1 << "P2" << P2;
	fs1 << "Q" << Q;
	cout << "Done Rectification\n";

	return 0;
}
