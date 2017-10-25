#include <iostream>
#include <vector>
#include <string>
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/core/core.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/calib3d/calib3d.hpp"
#include <opencv2/video/video.hpp>

using namespace std;
using namespace cv;

Mat img1, gray1, img2, gray2;
double x_center1, y_center1, x_center2, y_center2;
double x, y, z, w; // coordinates
double dist; // distance
Mat point1, point2; // center of the faces
Mat P1, P2, CM1, CM2, D1, D2, R1, R2;
Mat R, T, E, F, Q;
Mat imgU1, imgU2;
vector<Point> points1, points2; // points1 and points2 are stored selected points from images

void open_calib_file();

int main(){
	Mat points4D(4, 1, CV_64FC1);
	open_calib_file();
	vector<Rect>face1, face2; // keeps the rect information to draw
	string star = "*"; // for putText	
	CascadeClassifier cascade;
	VideoCapture cap1(0); // channel 1 for left camera
	VideoCapture cap2(1); // channel 2 for right camera
	cascade.load("/home/pi/opencv-3.1.0/data/haarcascades/haarcascade_frontalface_alt.xml");
	while(1){
	cap1 >> img1;
	cap2 >> img2;
	if (!img1.data || !img2.data) return -1;
	undistort(img1, imgU1, CM1, D1);
	undistort(img2, imgU2, CM2, D2);
	cvtColor(imgU1, gray1, CV_RGB2GRAY);
	cvtColor(imgU2, gray2, CV_RGB2GRAY);
imshow("FACE DETECTION cam1", imgU1);
imshow("FACE DETECTION cam2", imgU2);
	cascade.detectMultiScale(gray1, face1, 1.1, 3, 0, Size(50, 50), Size(200, 200));
	cascade.detectMultiScale(gray2, face2, 1.1, 3, 0, Size(50, 50), Size(200, 200));		
		if (face1.size()==1 && face2.size()==1){
			for (int i = 0; i < face1.size(); i++) {
				Rect r1 = face1.at(i);
				rectangle(imgU1, r1, CV_RGB(150, 185, 215), 2, 8, 0);
				x_center1 = r1.x + (r1.width / 2);
				y_center1 = r1.y + (r1.height / 2);
				double imagePoint1[] = { x_center1, y_center1};
				point1 = Mat(2, 1, CV_64FC1, imagePoint1);
				cout << point1 << endl;
				putText(imgU1, star, Point(x_center1, y_center1), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 1.0);
		}
	imshow("FACE DETECTION cam1", imgU1);
		for (int i = 0; i < face2.size(); i++) {
			Rect r2 = face2.at(i);
			rectangle(imgU2, r2, CV_RGB(150, 185, 215), 2, 8, 0);
			x_center2 = r2.x + (r2.width / 2);
			y_center2 = r2.y + (r2.height / 2);
			double imagePoint2[] = { x_center2, y_center2};
			point2 = Mat(2, 1, CV_64FC1, imagePoint2);
			cout << point2 << endl;
			putText(imgU2, star, Point(x_center2, y_center2), FONT_HERSHEY_SIMPLEX, 1, CV_RGB(0, 255, 0), 1.0);
		}
	imshow("FACE DETECTION cam2", imgU2);
	triangulatePoints(P1, P2, point1, point2, points4D);
	// points4D (x,y,z,w)
	w = points4D.at<double>(3, 0);
	x = points4D.at<double>(0, 0) / w;
	y = points4D.at<double>(1, 0) / w;
	z = points4D.at<double>(2, 0) / w;
	dist = sqrt(pow(x, 2) + pow(y, 2) + pow(z, 2));
	cout << "distance : " << dist  << " cm" << endl;
	cout << "triangulatePoints method: ";
	cout << "x : " << x << ", " << "y : " << y << ", " << "z : " << z << " w : " << w << endl;
	
	}// end of if to control there is a one face or not
	if (waitKey(30) >= 0) break;
	}//end of while
    	return 0;
}

void open_calib_file() {
	FileStorage file("mystereocalib.yml", FileStorage::READ);
	file["CM1"] >> CM1;
	file["CM2"] >> CM2;
	file["D1"] >> D1;
	file["D2"] >> D2;
	file["R"] >> R;
	file["T"] >> T;
	file["E"] >> E;
	file["F"] >> F;
	file["R1"] >> R1;
	file["R2"] >> R2;
	file["P1"] >> P1;
	file["P2"] >> P2;
	file["Q"] >> Q;
}
	