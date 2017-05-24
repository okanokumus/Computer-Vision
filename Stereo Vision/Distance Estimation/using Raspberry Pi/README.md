Same process will be applied using Raspberry Pi 3 and Rasbian as OS.
System hardware and requirements: 
* Raspberry Pi 3
* SD card (for OS and other data)
* OpenCV 3.1.0
* C++ language
* Two cameras
* Monitor
* HDMI to VGA converter (connection of monitor and pi)
* Mouse
* Keyboard
For more information about Raspberry Pi 3, you can visit https://www.raspberrypi.org/

To compile the codes, open current folder in the terminal(press F4 or you can access from 'tools') and write:
'g++ `pkg-config --cflags opencv` face.cpp `pkg-config --libs opencv` -o face'.(face is just an example you can write anything
that you want to compile) then system produces the exacutable file 'face'.
