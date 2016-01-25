#include "opencv/highgui.h"
#include "opencv2/opencv.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>
#include <conio.h>
#include <stdlib.h>
using namespace cv;
using namespace std;

char window_name[30] = "CMYK";
int CMYK[4];
Vec3b targetBGR;
int targetCMYK[4];
Point mousePosition = Point(320,240);
Mat captureImage;

void rgb2cmyk(const Vec3b bgr, int * cmyk) {
	int B = bgr.val[0];
	int G = bgr.val[1];
	int R = bgr.val[2];
	float r = R / 255.;
    float g = G / 255.;
    float b = B / 255.;
    float k = std::min(std::min(1- r, 1- g), 1- b);         
	
	cmyk[0] = (1 - r - k) / (1 - k) * 100.;
	cmyk[1] = (1 - g - k) / (1 - k) * 100.;
	cmyk[2] = (1 - b - k) / (1 - k) * 100.;
	cmyk[3] = k * 100.;
}

static void onMouse(int event, int x, int y, int f, void* userdata){
	if(event==CV_EVENT_LBUTTONDOWN){
		mousePosition.x = x;
		mousePosition.y = y;
	}
	else if(event==CV_EVENT_RBUTTONDOWN){
		mousePosition.x = x;
		mousePosition.y = y;
		rgb2cmyk(targetBGR, targetCMYK);
		targetBGR = captureImage.at<Vec3b>(y, x);
		rgb2cmyk(targetBGR, targetCMYK);
	}
}
Vec3b colorAverage(Mat image){
	int w = image.cols;
	int h = image.rows;
	int pixelNum = w * h;
	int B = 0;
	int G = 0;
	int R = 0;
	for(int i = 0; i < w; i++)
		for(int j = 0; j < h; j++){
			B += image.at<Vec3b>(j,i)[0];
			G += image.at<Vec3b>(j,i)[1];
			R += image.at<Vec3b>(j,i)[2];
		}
	//std::cout<<int(float(B)/float(pixelNum))<<std::endl; //blue mean
	//std::cout<<int(float(G)/float(pixelNum))<<std::endl; //green
	//std::cout<<int(float(R)/float(pixelNum))<<std::endl; //red
	return Vec3b(int(float(B)/float(pixelNum)),int(float(G)/float(pixelNum)),int(float(R)/float(pixelNum)));
}
bool DisplayInfo(const Mat image){
	system("cls");
	/*float target_sum=0, sum=0;
	for(int i=0; i<3; i++){
		target_sum+=float(targetCMYK[i]);
		sum+=float(CMYK[i]);
	}
	float target_ratio[3];
	float ratio[3];
	for(int i=0;i<3;i++){
		target_ratio[i] = float(targetCMYK[i])/target_sum;
		ratio[i] = float(CMYK[i])/sum;
	}
	printf ("*****************\n");
	printf (" Color Detection\n");
	printf ("*****************\n");
	printf (" Target: (%.2f,%.2f,%.2f)\n",target_ratio[0],target_ratio[1],target_ratio[2]);
	printf (" Detect: (%.2f,%.2f,%.2f)\n", ratio[0],ratio[1],ratio[2]);
	printf (" Differ: (%.2f,%.2f,%.2f)\n", target_ratio[0]-ratio[0],target_ratio[1]-ratio[1],target_ratio[2]-ratio[2]);
	*/

	char name[30];
	int w = image.cols, h = image.rows;
	int patch_size = 100;

	Vec3b rgb = image.at<Vec3b>(mousePosition.y, mousePosition.x);
	// Draw marker
	line(image, Point(mousePosition.x-10,mousePosition.y),Point(mousePosition.x+10,mousePosition.y),Scalar(0,0,255));
	line(image, Point(mousePosition.x,mousePosition.y-10),Point(mousePosition.x,mousePosition.y+10),Scalar(0,0,255));
	//rgb= colorAverage(image(Rect(mousePosition.x-5,mousePosition.y-5,10,10)));
	//rectangle(image, Point(mousePosition.x-5,mousePosition.y-5),Point(mousePosition.x+5,mousePosition.y+5),Scalar(0,255,255));
	
	rgb2cmyk(rgb, CMYK);
	sprintf(name, "C=%d", CMYK[0]);
	putText(image, name, Point(25, 40), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	sprintf(name, "M=%d", CMYK[1]);
	putText(image, name, Point(25, 80), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	sprintf(name, "Y=%d", CMYK[2]);
	putText(image, name, Point(25, 120), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	sprintf(name, "K=%d", CMYK[3]);
	putText(image, name, Point(25, 160), FONT_HERSHEY_SIMPLEX, .7, Scalar(0, 255, 0), 2, 8, false);

	rectangle( image, Point(w-2*patch_size, 0), Point(w-patch_size, patch_size), Scalar( rgb[0], rgb[1], rgb[2] ), -1, 8 );
	rectangle( image, Point(w-patch_size, 0), Point(w, patch_size), Scalar( targetBGR[0], targetBGR[1], targetBGR[2] ), -1, 8 );

	int Differ[3] = {targetCMYK[0]-CMYK[0],targetCMYK[1]-CMYK[1],targetCMYK[2]-CMYK[2]};

	printf ("*****************\n");
	printf (" Color Detection\n");
	printf ("*****************\n");
	printf (" Target: (%d,%d,%d)\n",targetCMYK[0],targetCMYK[1],targetCMYK[2]);
	printf (" Detect: (%d,%d,%d)\n", CMYK[0],CMYK[1],CMYK[2]);
	printf (" Differ: (%d,%d,%d)\n\n", Differ[0],Differ[1],Differ[2]);

	// Decide initial Color
	int colorToDraw = 0;
	int maxDiffer = 0;
	for(int i = 0; i < 3; i++){
		if(abs(int(Differ[i]))>maxDiffer){
			if(int(Differ[i]) > 0){
				colorToDraw = i;
			}else{
				colorToDraw = 3;
			}
			maxDiffer = abs(int(Differ[i]));
		}
	}

	if(maxDiffer<5)
		colorToDraw = 4;
	switch(colorToDraw){
		case 0:
			printf (" Draw: Cyan %d \n", maxDiffer);
			break;
		case 1:
			printf (" Draw: Magenta %d \n", maxDiffer);
			break;
		case 2:
			printf (" Draw: Yellow %d \n", maxDiffer);
			break;
		case 3:
			printf (" Draw: White %d \n", maxDiffer);
			break;
		case 4:
			printf (" Color mixing done! \n");
			break;
	}

	imshow(window_name, image);
	cvWaitKey(33);


	return false;
	/*if(maxDiffer>5)
		return false;
	else
		return true;*/
}
void CaptureFrame(){
	
	VideoCapture captureDevice;
	captureDevice.open(0);
	if(!captureDevice.isOpened()){
		cout<<"Camera not found!";
	}
	else{
		char kbCmd = ' ', name[30];
		captureDevice >> captureImage;
		int w = captureImage.cols, h = captureImage.rows;
		bool finish = false;
		while(!finish){
			setMouseCallback(window_name, onMouse, 0);
			captureDevice >> captureImage;
			finish = DisplayInfo(captureImage);
		
			if (_kbhit()) kbCmd = _getche();
			if (kbCmd == 'c') break;
		}
		destroyAllWindows();
	}
}
int main()
{
	// *********************
	//     Target color
	// *********************
	Mat colorPatch = imread("color_patch2.jpg");
	//imshow("targetRGB",targetRGB);
	//waitKey(0);
	targetBGR = colorPatch.at<Vec3b>(0, 0);
	
	rgb2cmyk(targetBGR, targetCMYK);
	CaptureFrame();
	

	return 0;
}
