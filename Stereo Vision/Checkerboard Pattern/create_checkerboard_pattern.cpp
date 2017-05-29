// create_checkerboard_pattern.cpp : Defines the entry point for the console application.
//
// to create own checkerboard pattern in the specific range of pixel values

#include "stdafx.h"
#include <iostream>
#include <Windows.h>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"

using namespace cv;
using namespace std;


int main(){
	int i, j, k, l, m, n, o, p;
	char filename[200];
	Mat img(500, 500, CV_8UC3, Scalar(0, 0, 0)); // size of the pattern will be 500x500 pixel
	int width = 5;
	int height = 5;
	o = 0;
	p = 0;
	for (n = 0; n < height; n++){
		for (i = 0; i < 100; i++){
			l = 0; m = p;
			for (j = 0; j < width; j++){
				for (k = 0; k < 100; k++){

					img.at<Vec3b>(i + o, k + l)[0] = m;
					img.at<Vec3b>(i + o, k + l)[1] = m;
					img.at<Vec3b>(i + o, k + l)[2] = m;
				}
				l += 100;
				if (m == 0)
					m = 255;
				else if (m == 255)
					m = 0;
			}
		}
		o += 100;
		if (p == 0)
			p = 255;
		else if (p == 255)
			p = 0;
	}
	namedWindow("pattern", WINDOW_AUTOSIZE);
	imshow("pattern", img);
	sprintf_s(filename, "C:/Users/okan/Desktop/ConsoleApplication1/my_pattern.png"); // addr of the wherever that you write
	imwrite(filename, img);
	waitKey(0);
	return 0;
}