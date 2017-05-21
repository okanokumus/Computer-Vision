Code is written in Visual Studio 2015 using OpenCV 3.1.0 library and C++ language.
Precompiled header file is used in Visual ( #include "stdafx.h" )
Firsty takes pictures from each channel and if they are proper for calibrtion saves in 'stere images/cam1/cam1_x' and 
'stereimages/cam2/cam2_x' folder. And the calibration process is continuing to process up to predetermined number of images. Finally 
intrinsic and extrinsic parameters of the cameras are saved in the 'mystereocalib.yml' file.
