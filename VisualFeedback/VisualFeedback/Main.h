#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <sstream>
#include <fstream>
#include <string>
#include <windows.h>
#include <opencv2/objdetect/objdetect.hpp>
#include <opencv2/objdetect/objdetect_c.h>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/highgui/highgui_c.h>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/imgproc/types_c.h>
#include <time.h> 
#include <opencv2/opencv.hpp>
#include <Eigen/Dense>
using namespace Eigen;
using namespace std;
using namespace cv;

#define DRAWTHRESH 30.0
#define ITERATION 5
#define SAMPLEFREQ 15
#define PAUSE fgetc(stdin);
#define EDGECLIPING 1
#define PI 3.14159
#define DISPLAY 0
#define SAVE 1
#define SIMULATION 1
#define CANNY 1

