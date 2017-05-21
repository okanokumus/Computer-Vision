clc
close all

current_folder=pwd; 
path1 = '\test images\cam1_2.png';
path2 = '\test images\cam2_2.png';

img1 = imread(sprintf('%s%s',current_folder,path1));
img2 = imread(sprintf('%s%s',current_folder,path2));

load('StereoParams6x9.mat');
I1 = undistortImage(img1,stereoParams6x9.CameraParameters1);
I2 = undistortImage(img2,stereoParams6x9.CameraParameters2);

detector_face = vision.CascadeObjectDetector;
% detector_face = vision.CascadeObjectDetector('MergeThreshold',8);
face1 = step(detector_face,I1);
face2 = step(detector_face,I2);

x1 = face1(1,1);
y1 = face1(1,2);
width1 = face1(1,3);
height1 = face1(1,4);
x_center1  = x1 + width1/2;
y_center1  = y1 + height1/2;
center1(1,:) = [x_center1 y_center1];

x2 = face2(1,1);
y2 = face2(1,2);
width2 = face2(1,3);
height2 = face2(1,4);
x_center2  = x2 + width2/2;
y_center2  = y2 + height2/2 +10.5;
center2(1,:) = [x_center2 y_center2];

point3d = triangulate(center1, center2, stereoParams6x9);
distanceInMeters = norm(point3d)/1000;


distanceAsString = sprintf('%0.2f meters', distanceInMeters);
I1 = insertObjectAnnotation(I1,'rectangle',face1,distanceAsString,'FontSize',18);
I2 = insertObjectAnnotation(I2,'rectangle',face2, distanceAsString,'FontSize',18);
I1 = insertShape(I1,'FilledRectangle',face1);
I2 = insertShape(I2,'FilledRectangle',face2);

imshowpair(I1, I2, 'montage');