// Stereo_Calibration_Capturing_Image.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <stdio.h>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>


using namespace std;
using namespace cv;

int main()
{
	int x = 0;
	char* extension = ".png"; // png format is the best for calibration becaues of png is the uncompressed format

	// video stream
	VideoCapture cap1(1); // cam1 is assigned as left camara
	VideoCapture cap2(2); // cam2 is assigned as right camara

	// image containers. img_res1 and img_res2 are for resized of the original images
	Mat img1, img_res1, img2, img_res2;

	 // endless loop to take images
	while (1) {
		cap1 >> img1; // for left camera
		cap2 >> img2; // for right camera
		// sets and resizes the properties of image
		cap1.set(CV_CAP_PROP_FRAME_WIDTH, 640);
		cap1.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
		resize(img1, img_res1, Size(640, 480));
		resize(img2, img_res2, Size(640, 480));

		imshow("IMG1", img_res1);
		imshow("IMG2", img_res2);

		// press 'SPACE' to take images
		if (waitKey(30) > 0) {
			x++;
			char filename1[200], filename2[200];
			// specify directory folders and the name of the images
			// write the images to the address of the files
			sprintf_s(filename1, "E:/visual stduio codes/image processing codes/Stereo_Calibration_Read_Image/Left_Camera/left_%d%s", x, extension); 
			// sprintf_s(filename1, "(current direction of the left camera)/left_%d%s", x, extension); 
			sprintf_s(filename2, "E:/visual stduio codes/image processing codes/Stereo_Calibration_Read_Image/Right_Camera/right_%d%s", x, extension);
 			// sprintf_s(filename1, "(current direction of the right camera)/right_%d%s", x, extension);
			cout << "Saving img pair " << x << endl;
			
			imwrite(filename1, img_res1);
			imwrite(filename2, img_res2);
		}
	}
	return 0;
}
