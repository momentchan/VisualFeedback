#include "FuncDeclaration.h"

//static void onMouse(int event, int x, int y, int f, void* userdata){
//	if (event == CV_EVENT_LBUTTONDOWN){
//		mousePosition.x = x;
//		mousePosition.y = y;
//	}
//	else if (event == CV_EVENT_RBUTTONDOWN){
//		mousePosition.x = x;
//		mousePosition.y = y;
//		rgb2cmyk(targetBGR, targetCMYK);
//		targetBGR = captureImage.at<Vec3b>(y, x);
//		rgb2cmyk(targetBGR, targetCMYK);
//	}
//}

void CaptureFrame(Mat & detectImg){
	VideoCapture captureDevice;
	captureDevice.open(0);
	if (!captureDevice.isOpened()){
		cout << "Camera not found!";
	}
	else{
		char kbCmd = ' ';
		captureDevice >> detectImg;
		int w = detectImg.cols, h = detectImg.rows;
		bool finish = false;
		while (!finish){
			captureDevice >> detectImg;
			detectImg = detectImg(Rect(120, 40, 400, 400));

			imshow("Capture", detectImg);
			cvWaitKey(33);
			if (_kbhit()) kbCmd = _getche();
			if (kbCmd == 'c') break;
		}
		destroyAllWindows();
	}
	//VideoCapture captureDevice;
	//captureDevice.open(0);
	//if (!captureDevice.isOpened()){
	//	cout << "Camera not found!";
	//}
	//else{
	//	char kbCmd = ' ', name[30];
	//	captureDevice >> captureImage;
	//	int w = captureImage.cols, h = captureImage.rows;
	//	bool finish = false;
	//	while (!finish){
	//		setMouseCallback(window_name, onMouse, 0);
	//		captureDevice >> captureImage;
	//		finish = DisplayInfo(captureImage);

	//		if (_kbhit()) kbCmd = _getche();
	//		if (kbCmd == 'c') break;
	//	}
	//	destroyAllWindows();
	//}
}