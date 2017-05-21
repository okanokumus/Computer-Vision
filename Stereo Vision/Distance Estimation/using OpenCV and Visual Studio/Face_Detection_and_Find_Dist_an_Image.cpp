// Face_Detection_and_Find_Dist_an_Image.cpp : Defines the entry point for the console application.
//


#include "stdafx.h"
#include <iostream>
#include <vector>
#include <sstream>
#include <time.h>
#include <stdio.h>
#include <opencv2\objdetect\objdetect.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\calib3d\calib3d.hpp>

using namespace std;
using namespace cv;

Mat img1, gray1, img2, gray2;
double x_center1, y_center1, x_center2, y_center2;
double x, y, z, w; // coordinates
double dist; // distance
Mat point1, point2, point22, point11; // center of the faces
FileStorage file;
Mat P1, P2, CM1, CM2, D1, D2, R1, R2;
Mat R, T, E, F, Q;
Mat imgU1, imgU2;
vector<Point> points1, points2; // points1 and points2 are stored selected points from images
void open_calib_file();
Mat triangulate_Linear_LS(Mat mat_P_l, Mat mat_P_r, Mat warped_back_l, Mat warped_back_r);

// main funciton
int main(int, char**) {
	Mat points4D(4, 1, CV_64FC1);
	open_calib_file(); // reads the my setereo calibration file and to get parameters
	img1 = imread("C:/Users/okan/Desktop/Stereo_Vision_MATLAB/test images/cam2_2.png", 1);
	img2 = imread("C:/Users/okan/Desktop/Stereo_Vision_MATLAB/test images/cam1_2.png", 1);
	//img1 = imread("C:/Users/okan/Desktop/Stereo_Vision_MATLAB/picasso_85cm/cam1/cam1_1.png", 1);
	//img2 = imread("C:/Users/okan/Desktop/Stereo_Vision_MATLAB/picasso_85cm/cam2/cam2_1.png", 1);
	if (!img1.data || !img2.data) return -1;
	undistort(img1, imgU1, CM1, D1);
	undistort(img2, imgU2, CM2, D2);
	cvtColor(imgU1, gray1, CV_RGB2GRAY);
	cvtColor(imgU2, gray2, CV_RGB2GRAY);
	/*gray1.convertTo(gray1, CV_64FC1, 1, 0); // convert the gray1 to the double type
	gray2.convertTo(gray2, CV_64FC1, 1, 0); // convert the gray1 to the double type*/
	vector<Rect>face1, face2; // keeps the rect information to draw
	CascadeClassifier cascade;
	// address of the cascade classifier (...data/haarcascades/haarcascade_frontalcatface.xml)
	cascade.load("E:/Opencv-3.1.0/data/haarcascades/haarcascade_frontalface_alt.xml");
	if (cascade.empty()) { // check the classifier is loaded or not
		cout << "cannot load cascade file" << endl;
		exit(-1);
	}
	cascade.detectMultiScale(gray1, face1, 1.1, 1, 0, Size(50, 50), Size(250, 250));
	cascade.detectMultiScale(gray2, face2, 1.1, 1, 0, Size(50, 50), Size(250, 250));

	for (int i = 0; i < face1.size(); i++) {
		Rect r1 = face1.at(i);
		rectangle(imgU1, r1, CV_RGB(150, 185, 215), 2, 8, 0);
		x_center1 = r1.x + (r1.width / 2);
		y_center1 = r1.y + (r1.height / 2);
		double imagePoint11[] = { x_center1, y_center1 };
		double imagePoint1[] = { x_center1, y_center1, 1};
		//cout << x_center1 << "  " << y_center1<<endl;
		point11 = Mat(2, 1, CV_64FC1, imagePoint11);
		point1 = Mat(3, 1, CV_64FC1, imagePoint1);
		cout << point1 << endl;
	}
	imshow("FACE DETECTION cam1", imgU1);

	for (int i = 0; i < face2.size(); i++) {
		Rect r2 = face2.at(i);
		rectangle(imgU2, r2, CV_RGB(150, 185, 215), 2, 8, 0);
		x_center2 = r2.x + (r2.width / 2);
		y_center2 = r2.y + (r2.height / 2);
		double imagePoint22[] = { x_center2, y_center2};
		double imagePoint2[] = { x_center2, y_center2,1 };
		point22 = Mat(2, 1, CV_64FC1, imagePoint22);
		point2 = Mat(3, 1, CV_64FC1, imagePoint2);
		cout << point2 << endl;
	}
	imshow("FACE DETECTION cam2", imgU2);


	points4D = triangulate_Linear_LS(P1, P2, point1, point2);
	w = points4D.at<double>(3, 0);
	x = points4D.at<double>(0, 0) / w;
	y = points4D.at<double>(1, 0) / w;
	z = points4D.at<double>(2, 0) / w;
	dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	cout << "distance : " << dist << "m" << endl;
	cout << "Least Square triangulation method: ";
	cout << "x : " << x << ", " << "y : " << y << ", " << "z : " << z << " w : " << w << endl;

	triangulatePoints(P1, P2, point11, point22, points4D);
	// points4D (x,y,z,w)
	w = points4D.at<double>(3, 0);
	x = points4D.at<double>(0, 0) / w;
	y = points4D.at<double>(1, 0) / w;
	z = points4D.at<double>(2, 0) / w;
	dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	cout << "distance : " << dist  << "m" << endl;
	cout << "triangulatePoints method: ";
	cout << "x : " << x << ", " << "y : " << y << ", " << "z : " << z << " w : " << w << endl;
	
	waitKey(0);
	return 0;
}

void open_calib_file() {
	file.open("E:/visual stduio codes/image processing codes/stereo vision/stereo_camera_calib_using_images/stereo_camera_calib_using_images/mystereocalib.yml", FileStorage::READ);
	// to get projection matrices and some other matrices for required undistortion

	file["CM1"] >> CM1;
	file["CM2"] >> CM2;
	file["D1"] >> D1;
	file["D2"] >> D2;
	file["R"] >> R;
	file["T"] >> T;
	file["E"] >> E;
	file["F"] >> F;
	/*
	Results of calibration:
	CM1 - Camera Matrix of first camera.
	CM2 - Camera Matrix of second camera.
	D1 - Distortion coeff matrix of first camera.
	D2 - Distortion coeff matrix of second camera.
	R - Rotation Matrix between first and second camera coordinate systems.
	T - Translation vector between the coordinate systems of the cameras.
	E - Essential matrix.
	F - Fundamental matrix.
	*/
	file["R1"] >> R1;
	file["R2"] >> R2;
	file["P1"] >> P1;
	file["P2"] >> P2;
	file["Q"] >> Q;
	/*
	Results of rectification:
	R1 - 3x3 rectification transform(rotation matrix) for the first camera.
	R2 - 3x3 rectification transform(rotation matrix) for the second camera.
	P1 - 3x4 projection matrix in the new (rectified) coordinate systems for the first camera.
	P2 - 3x4 projection matrix in the new (rectified) coordinate systems for the second camera.
	Q – 4x4 disparity - to - depth mapping matrix.
	*/
}

Mat triangulate_Linear_LS(Mat mat_P_l, Mat mat_P_r, Mat warped_back_l, Mat warped_back_r)
{

	Mat A(4, 3, CV_64FC1), b(4, 1, CV_64FC1), X(3, 1, CV_64FC1), W(1, 1, CV_64FC1), points4D(4, 1, CV_64FC1);

	W.at<double>(0, 0) = 1.0;
	A.at<double>(0, 0) = (warped_back_l.at<double>(0, 0) / warped_back_l.at<double>(2, 0))*mat_P_l.at<double>(2, 0) - mat_P_l.at<double>(0, 0);
	A.at<double>(0, 1) = (warped_back_l.at<double>(0, 0) / warped_back_l.at<double>(2, 0))*mat_P_l.at<double>(2, 1) - mat_P_l.at<double>(0, 1);
	A.at<double>(0, 2) = (warped_back_l.at<double>(0, 0) / warped_back_l.at<double>(2, 0))*mat_P_l.at<double>(2, 2) - mat_P_l.at<double>(0, 2);
	A.at<double>(1, 0) = (warped_back_l.at<double>(1, 0) / warped_back_l.at<double>(2, 0))*mat_P_l.at<double>(2, 0) - mat_P_l.at<double>(1, 0);
	A.at<double>(1, 1) = (warped_back_l.at<double>(1, 0) / warped_back_l.at<double>(2, 0))*mat_P_l.at<double>(2, 1) - mat_P_l.at<double>(1, 1);
	A.at<double>(1, 2) = (warped_back_l.at<double>(1, 0) / warped_back_l.at<double>(2, 0))*mat_P_l.at<double>(2, 2) - mat_P_l.at<double>(1, 2);
	A.at<double>(2, 0) = (warped_back_r.at<double>(0, 0) / warped_back_r.at<double>(2, 0))*mat_P_r.at<double>(2, 0) - mat_P_r.at<double>(0, 0);
	A.at<double>(2, 1) = (warped_back_r.at<double>(0, 0) / warped_back_r.at<double>(2, 0))*mat_P_r.at<double>(2, 1) - mat_P_r.at<double>(0, 1);
	A.at<double>(2, 2) = (warped_back_r.at<double>(0, 0) / warped_back_r.at<double>(2, 0))*mat_P_r.at<double>(2, 2) - mat_P_r.at<double>(0, 2);
	A.at<double>(3, 0) = (warped_back_r.at<double>(1, 0) / warped_back_r.at<double>(2, 0))*mat_P_r.at<double>(2, 0) - mat_P_r.at<double>(1, 0);
	A.at<double>(3, 1) = (warped_back_r.at<double>(1, 0) / warped_back_r.at<double>(2, 0))*mat_P_r.at<double>(2, 1) - mat_P_r.at<double>(1, 1);
	A.at<double>(3, 2) = (warped_back_r.at<double>(1, 0) / warped_back_r.at<double>(2, 0))*mat_P_r.at<double>(2, 2) - mat_P_r.at<double>(1, 2);
	b.at<double>(0, 0) = -((warped_back_l.at<double>(0, 0) / warped_back_l.at<double>(2, 0))*mat_P_l.at<double>(2, 3) - mat_P_l.at<double>(0, 3));
	b.at<double>(1, 0) = -((warped_back_l.at<double>(1, 0) / warped_back_l.at<double>(2, 0))*mat_P_l.at<double>(2, 3) - mat_P_l.at<double>(1, 3));
	b.at<double>(2, 0) = -((warped_back_r.at<double>(0, 0) / warped_back_r.at<double>(2, 0))*mat_P_r.at<double>(2, 3) - mat_P_r.at<double>(0, 3));
	b.at<double>(3, 0) = -((warped_back_r.at<double>(1, 0) / warped_back_r.at<double>(2, 0))*mat_P_r.at<double>(2, 3) - mat_P_r.at<double>(1, 3));
	solve(A, b, X, DECOMP_SVD);
	vconcat(X, W, points4D);
	return points4D;
}