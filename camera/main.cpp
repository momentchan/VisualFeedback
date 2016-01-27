#include "main.h"
#include "FuncDeclaration.h"
char window_name[30] = "CMYK";
int CMYK[4];
Vec3b targetBGR;
int targetCMYK[4];
Point mousePosition = Point(320,240);
Mat captureImage;

Mat targetImg;
void rgb2cmyk(const Vec3b bgr, int * cmyk) {
	int B = bgr.val[0];
	int G = bgr.val[1];
	int R = bgr.val[2];
	float r = R / 255.;
    float g = G / 255.;
    float b = B / 255.;
    float k = std::min(std::min(1- r, 1- g), 1- b);         
	
	cmyk[0] = (1 - r - k) / (1 - k) * 255.;
	cmyk[1] = (1 - g - k) / (1 - k) * 255.;
	cmyk[2] = (1 - b - k) / (1 - k) * 255.;
	cmyk[3] = k * 255.;
}
void rgb2cmyk(const Vec3b bgr, Vec4b &cmyk) {
	int B = bgr.val[0];
	int G = bgr.val[1];
	int R = bgr.val[2];
	float r = R / 255.;
    float g = G / 255.;
    float b = B / 255.;
    float k = std::min(std::min(1- r, 1- g), 1- b);         
	
	cmyk = Vec4b((1 - r - k) / (1 - k) * 255., (1 - g - k) / (1 - k) * 255., (1 - b - k) / (1 - k) * 255., k * 255.);
	//cmyk[0] = (1 - r - k) / (1 - k) * 100.;
	//cmyk[1] = (1 - g - k) / (1 - k) * 100.;
	//cmyk[2] = (1 - b - k) / (1 - k) * 100.;
	//cmyk[3] = k * 100.;
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
Mat colorImg;

// RGB to CMYK conversion
void colorDiffer(Mat target, Mat detect) {

    
    // rgb to cmyk
	Mat differMap = target;
	/*Mat differMap = Mat(target.size(), CV_8UC3);
	differMap.setTo(255);*/
	/*imshow("",differMap);
	waitKey(0);*/
    for (int y = 0; y < target.rows; y++) {
        for (int  x = 0; x < target.cols; x++) {
			int targetCMYK[4];
			int detectCMYK[4];
			rgb2cmyk(target.at<Vec3b>(y, x),targetCMYK);
			rgb2cmyk(detect.at<Vec3b>(y, x),detectCMYK);
			ArrayXXf differ(1,3);
			//cout<<targetCMYK[0]<<" "<<targetCMYK[1]<<" "<<targetCMYK[2]<<" "<<endl;
			//cout<<detectCMYK[0]<<" "<<detectCMYK[1]<<" "<<detectCMYK[2]<<" "<<endl;
			differ << abs(targetCMYK[0]-detectCMYK[0]), abs(targetCMYK[1]-detectCMYK[1]), abs(targetCMYK[2]-detectCMYK[2]);
			//cout<<differ.maxCoeff()<<endl;
			//cout<<Scalar(0,0,differ.maxCoeff())<<endl;
			if(differ.maxCoeff()>25)
				circle(differMap, Point(x, y), 0, Scalar(0,0,differ.maxCoeff()));
			//differMap.at<Vec3b>(i, j)[2] = (uchar)differ.maxCoeff();
			//Vec4b differ = targetCMYK-detectCMYK;

        }
    }
	imshow("",differMap);
	waitKey(0);
}
void test(int &a){
	cout<<a<<endl;
}
int main()
{
	//// *********************
	////     Target color
	//// *********************
	//Mat colorPatch = imread("color_patch2.jpg");
	////imshow("targetRGB",targetRGB);
	////waitKey(0);
	//targetBGR = colorPatch.at<Vec3b>(0, 0);
	//
	//rgb2cmyk(targetBGR, targetCMYK);
	//CaptureFrame();

	targetImg = imread("colorImgAlexis.jpg");
	Mat detectImg = imread("OriginColorSegment2.jpg");
    colorDiffer(targetImg, detectImg);
 
    return 0;
	//ColorSeparation();
}
