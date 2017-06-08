// stereo_camera_calib_using_images.cpp : Defines the entry point for the console application.
//
// camera calibration : http://docs.opencv.org/2.4/modules/calib3d/doc/camera_calibration_and_3d_reconstruction.html#double 
// calibrationMatrixValues : http://docs.opencv.org/3.1.0/d9/d0c/group__calib3d.html#ga87955f4330d5c20e392b265b7f92f691

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <opencv2\core\core.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\calib3d\calib3d.hpp>

using namespace std;
using namespace cv;

vector< vector< Point3f > > object_points; // the actual 3D coordinate of those checkerboard points. 
/*objectPoints should contain physical location of each corners but since we don’t know 
that so we assign constant positions to all the corners and assume that camera is moving.*/
vector< vector< Point2f > > imagePoints1, imagePoints2; //checkerboard corner coordinates in the image 
vector< Point2f > corners1, corners2;
vector< vector< Point2f > > left_img_points, right_img_points;
vector<Point3f> obj;
vector<int> rejected; // will store the rejected number of images
Mat img1, gray1, img2, gray2; // images and garyscale containers of them

FileStorage fs("mystereocalib.yml", FileStorage::WRITE); // save stereo parameters as a .yml file into the same folder
FileStorage fs1("FirstCamera.yml", FileStorage::WRITE); // save First cam parameters as a .yml file into the same folder
FileStorage fs2("SecondCamera.yml", FileStorage::WRITE); // save Secod cam parameters as a .yml file into the same folder

struct CamMatrix {
	// first(LEFT) and second (RIGHT) camera properties that will find using camera matrix : 
	double 	apertureWidth, apertureHeight, fovx, fovy, focalLength, aspectRatio;
	Size Size;
	Point2d principalPoint;
	/*	
		apertureWidth: Physical width in mm of the sensor.
		apertureHeight: Physical height in mm of the sensor.
		fovx: Output field of view in degrees along the horizontal sensor axis.
		fovy: Output field of view in degrees along the vertical sensor axis.
		focalLength: Focal length of the lens in mm.
		principalPoint: Principal point in mm.
		aspectRatio: fy / fx

	*/
} LeftCam, RightCam;


int main()
{
	int no_images = 40; // number of images
	char* extension = ".png";
	char* address1 = "C:/Users/okan/Desktop/Stereo_Vision_MATLAB/calibration pictures/cam1/"; // left camera
	char* address2 = "C:/Users/okan/Desktop/Stereo_Vision_MATLAB/calibration pictures/cam2/"; // right camera
	int success = 0, k = 0;
	int board_height = 6; // the number of internal corners vertically
	int board_width = 9; // the number of internal corners horizontally
	int board_n = board_width*board_height; // number of squares
	float square_dimension = 0.035f; // m
	Size board_size = Size(9,6);
	

/*	for (int j = 0; j<board_n; j++){
		// This creates a list of coordinates (0,0,0), (0,1,0), (0,2,0)...(1,4,0)... so on. 
		// Each corresponds to a particular vertex.
		obj.push_back(Point3f(j / board_width, j%board_width, 0.0f));
	}
*/
	for (int i = 0; i < board_height; i++)
		for (int j = 0; j < board_width; j++)
			obj.push_back(Point3f((float)j * square_dimension, (float)i * square_dimension, 0));

	for (int i = 1; i <= no_images; i++) {
		// read the both channels images
		char filename1[200], filename2[200];
		sprintf_s(filename1, "%scam1_%d%s", address1, i, extension);// name and extension of the iamges for the left camera
		sprintf_s(filename2, "%scam2_%d%s", address2, i, extension);// name and extension of the iamges for the right camera
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

		/*If corners are detected, they are further refined.
		Subpixel corners are calculated from the grayscale image.
		This is an iterative process, so you need to provide a termination criteria
		(number of iterations, amount of error allowed, etc).*/
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
		/*if (found1 && found2);	k = waitKey(0);*/ // to see each found corners individually
		if (k == 27)break;
		if (!found1 || !found2)
			rejected.push_back(i);
		if (found1 && found2)
		{

			imagePoints1.push_back(corners1);
			imagePoints2.push_back(corners2);
			object_points.push_back(obj);
			cout << "Corners stored from the "<< i <<". image" << endl;
			success++;

			if (success >= no_images)break;
		}
	}
	destroyAllWindows();
	
	Mat CM1, CM2;
	Mat D1, D2;
	Mat R, T, E, F;
	for (int i = 0; i < rejected.size(); i++) {
		cout << rejected.size() << " Rejected image(s): \n";
		cout << rejected.at(i) << ". image\n";
	}
	std::cout << "********\n";

	std::cout << success << " pairs of stereo images will be used for calibration !!! " << endl;

	std::cout << "********\n";

	std::cout << "Starting Calibration\n";

	// camera calibration individually for Camera matrices and distortion coeff.
	vector< Mat > rvecs, tvecs; 
	// to ignore higher order distortion coefficients k4 and k5.
	int flag = 0;
	flag |= CV_CALIB_FIX_K4;
	flag |= CV_CALIB_FIX_K5;

	std::cout << "********\n";

	cout << "first camera calibration is started" << endl;
	// first camera (left cam)
	fs1 << "Left camera parameters " << "fisrt camera";
	double rms_cam1 = calibrateCamera(object_points, imagePoints1, img1.size(), CM1, D1, rvecs, tvecs, flag);
	fs1 << "Left camera calibration error" << rms_cam1;  
	cout << "Calibration error of the left camera: " << rms_cam1 << endl; // rms error for left camera calibration
	// Computes useful camera characteristics from the camera matrix.
	cv::calibrationMatrixValues(CM1, img1.size(), LeftCam.apertureWidth, LeftCam.apertureHeight,
		LeftCam.fovx, LeftCam.fovy, LeftCam.focalLength, LeftCam.principalPoint, LeftCam.aspectRatio);
	fs1 << "Output field of view in degrees along the horizontal  sensor axis" << LeftCam.fovx;
	fs1 << "Output field of view in degrees along the vertical  sensor axis" << LeftCam.fovy;
	fs1 << "Physical width in mm of the sensor" << LeftCam.apertureWidth ;
	fs1 << "Physical height in mm of the sensor" << LeftCam.apertureHeight;
	fs1 << "Focal length" << LeftCam.focalLength;
	fs1 << "Principal point" << LeftCam.principalPoint;
	fs1 << "Aspect ratio fy divide by fx" << LeftCam.aspectRatio;
	fs1 << "CM1" << CM1; //CM1 - Camera Matrix of first camera
	fs1 << "D1" << D1; //D1 - Distortion coeff matrix of first camera
	fs << "CM1" << CM1; //CM1 - Camera Matrix of first camera
	fs << "D1" << D1; //D1 - Distortion coeff matrix of first camera
	cout << "first camera calibration is finished and parameters are saved calibration file" << endl;
	
	std::cout << "********\n";
	
	// second camera (right cam)
	cout << "second camera calibration is started" << endl;
	fs2 << "Right camera parameters" << "second camera";
	double rms_cam2 = calibrateCamera(object_points, imagePoints2, img2.size(), CM2, D2, rvecs, tvecs, flag);
	fs2 << "Right camera calibration error " << rms_cam2; // rms error for right camera calibration
	cout << "Calibration error of the right camera: " << rms_cam2 << endl;
	// Computes useful camera characteristics from the camera matrix.
	cv::calibrationMatrixValues(CM1, img2.size(), RightCam.apertureWidth, RightCam.apertureHeight,
		RightCam.fovx, RightCam.fovy, RightCam.focalLength, RightCam.principalPoint, RightCam.aspectRatio);

	fs2 << "Output field of view in degrees along the horizontal  sensor axis" << RightCam.fovx;
	fs2 << "Output field of view in degrees along the vertical  sensor axis" << RightCam.fovy;
	fs2 << "Physical width in mm of the sensor" << RightCam.apertureWidth;
	fs2 << "Physical height in mm of the sensor" << RightCam.apertureHeight;
	fs2 << "Focal length" << RightCam.focalLength;
	fs2 << "Principal point" << RightCam.principalPoint;
	fs2 << "Aspect ratio fy divide by fx" << RightCam.aspectRatio;
	fs2 << "CM2" << CM2; // CM2 - Camera Matrix of second camera
	fs2 << "D2" << D2; // D2  - Distortion coeff matrix of second camera
	fs << "CM2" << CM2; // CM2 - Camera Matrix of second camera
	fs << "D2" << D2; // D2  - Distortion coeff matrix of second camera
	cout << "second camera calibration is finished and parameters are saved calibration file" << endl;
	
	std::cout << "********\n";
	
	cout << "stereo calibration is started" << endl;
	double rms_stereo = stereoCalibrate(object_points, imagePoints1, imagePoints2,
		CM1, D1, CM2, D2, img1.size(), R, T, E, F,
		(TermCriteria::MAX_ITER + TermCriteria::EPS, cv::CALIB_FIX_INTRINSIC, 1e-5));
	cout << "rms error of the stereo camera calibraiton : " << rms_stereo << endl;
	fs << "R" << R; // R - Rotation Matrix between first and second camera coordinate systems.
	fs << "T" << T; // T - Translation vector between the coordinate systems of the cameras.
	fs << "E" << E; // E - Essential matrix.
	fs << "F" << F; // F - Fundamental matrix.
	fs << "Stereo calibration error" << rms_stereo; // rms error for stereo camera calibration
	cout << "Done Calibration\n";
	
	std::cout << "********\n";

	cout << "Starting Rectification\n";
	Mat R1, R2, P1, P2, Q;
	stereoRectify(CM1, D1, CM2, D2, img1.size(), R, T, R1, R2, P1, P2, Q, 0, -1);
	fs << "R1" << R1; // R1 - 3x3 rectification transform(rotation matrix) for the first camera.
	fs << "R2" << R2; // R2 - 3x3 rectification transform(rotation matrix) for the second camera.
	fs << "P1" << P1; // P1 - 3x4 projection matrix in the new (rectified) coordinate systems for the first camera.
	fs << "P2" << P2; // P2 - 3x4 projection matrix in the new (rectified) coordinate systems for the second camera.
	fs << "Q" << Q; // Q – 4x4 disparity - to - depth mapping matrix.
	cout << "Done Rectification\n";

	return 0;
}
