#include <vector>
#include <iostream>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"

using namespace std;
using namespace cv;



int main(){
	Mat img, g_img;
	vector<Rect>face; // keeps the rect information to draw
	CascadeClassifier cascade;
	img = imread("/home/pi/Desktop/Opencv/c++/face.jpg",1);
	cvtColor(img, g_img, CV_RGB2GRAY);
	cascade.load("/home/pi/opencv-3.1.0/data/haarcascades/haarcascade_frontalface_alt.xml");
	cascade.detectMultiScale(g_img, face, 1.1, 3, 0, Size(30, 30), Size(100, 100));
	for (int i = 0; i < face.size(); i++) {
		Rect r = face.at(i);
		rectangle(img, r , CV_RGB(150, 185, 215), 2,8, 0);
	}
	namedWindow("detected faces in the image", 1);
	imshow("detected faces in the image", img);
	waitKey(0);
    	return 0;
}
