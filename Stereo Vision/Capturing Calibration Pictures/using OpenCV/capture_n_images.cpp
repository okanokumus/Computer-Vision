// capture_n_images.cpp : Defines the entry point for the console application.
// 
//capture pictures

#include "stdafx.h"
#include <iostream>
#include <stdio.h>
#include "opencv2\imgproc\imgproc.hpp"
#include <opencv2\highgui\highgui.hpp>
#include <opencv2\video\video.hpp>

using namespace std;
using namespace cv;

Mat img1, img_res1, img2, img_res2;
char* extension = ".png";
int main()
{
	int i = 0;
	int no_images = 10; // enter how many images that capture
	VideoCapture cap1(1);
	VideoCapture cap2(2);
	while(i<no_images) {
		cap1 >> img1;
		cap2 >> img2;


		imshow("cam1", img1);
		imshow("cam2", img2);
		if (waitKey(30) > 0) {
			i++;
			char filename1[200], filename2[200];
			// specify directory folders and the name of the images
			// write the images to the address of the files
			sprintf_s(filename1, "C:/Users/okan/Desktop/images/cam1/cam1_hh%d%s", i, extension);
			// sprintf_s(filename1, "(current direction of the left camera)/left_%d%s", x, extension); 
			sprintf_s(filename2, "C:/Users/okan/Desktop/images/cam2/cam2_hh%d%s", i, extension);
			// sprintf_s(filename1, "(current direction of the right camera)/right_%d%s", x, extension);
			cout << "Saving img pair " << i << endl;

			imwrite(filename1, img1);
			imwrite(filename2, img2);
		}

	}
    return 0;
}

