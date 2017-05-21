% image capturing from two cameras
clc
clear all
close all
webcamlist % lists the all connected cameras
% according to your computer, number of cam could be changed
cam1 = webcam(1); % first usb camera 
cam3 = webcam(3); % second usb camera
current_folder=pwd; % current folder of the MATLAB
% create folders in the current folder
path_1 = [current_folder '\cam1\'];
path_3 = [current_folder '\cam3\'];
nof_images=20; % number of images that use in calibration. between 10-20 images are sufficient

% preview of the cameras
f1=preview(cam1);
movegui(f1,'east')
f3=preview(cam3);
movegui(f3,'west')


for i = 1:nof_images
    pause
    img1 = snapshot(cam1);
    img3 = snapshot(cam3);
    imwrite(img1, [path_1 'cam1_' int2str(i) '.png'],'png');
    imwrite(img3, [path_3 'cam3_' int2str(i) '.png'],'png');
    imshowpair(img1, img3, 'montage');
end
