#include <iostream>
#include <cstring>
#include <stdio.h>
#include "opencv2/imgproc/imgproc.hpp"
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/video/video.hpp>

using namespace std;
using namespace cv;

Mat img1, img_res1, img2, img_res2;
int main()
{
	int i = 0;
	int no_images = 5; // enter how many images that capture
	VideoCapture cap1(2);
	VideoCapture cap2(3);
	while (i<no_images) {
		cap1 >> img1;
		cap2 >> img2;

		imshow("cam1", img1);
		imshow("cam2", img2);
		if (waitKey(30) > 0) {
			i++;
			char filename1[200], filename2[200];
			// specify directory folders and the name of the images
			// write the images to the address of the files
			sprintf(filename1, "/home/pi/Desktop/using Raspberry Pi/take pictures/cam1/cam1_%d.png", i);
			// sprintf_s(filename1, "(current direction of the left camera)/left_%d%s", x, extension); 
			sprintf(filename2, "/home/pi/Desktop/using Raspberry Pi/take pictures/cam2/cam2_%d.png", i);
			// sprintf_s(filename1, "(current direction of the right camera)/right_%d%s", x, extension);
			cout << "Saving img pair " << i << endl;

			imwrite(filename1, img1);
			imwrite(filename2, img2);
		}

	}
	return 0;
}
