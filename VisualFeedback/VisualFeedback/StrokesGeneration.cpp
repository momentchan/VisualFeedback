#include "FuncDeclaration.h"
#include "DataStructure.h"

void SobelDetection(Mat src, Mat & grad, Mat & angles){
	int scale = 1, delta = 0;
	int ddepth = CV_16S;
	int c;
	Mat src_gray;
	GaussianBlur(src, src, Size(3, 3), 0, 0);
	cvtColor(src, src_gray, CV_BGR2GRAY);

	/// Generate grad_x and grad_y
	Mat abs_grad_x, abs_grad_y;
	Mat grad_x, grad_y;
	/// Gradient X
	Sobel(src_gray, grad_x, ddepth, 1, 0, 3, scale, delta);
	convertScaleAbs(grad_x, abs_grad_x);
	/// Gradient Y
	Sobel(src_gray, grad_y, ddepth, 0, 1, 3, scale, delta);
	convertScaleAbs(grad_y, abs_grad_y);
	/// Total Gradient (approximate)
	//addWeighted(abs_grad_x, 0.5, abs_grad_y, 0.5, 0, grad);
	for (int y = 0; y < grad.rows; y++)
	for (int x = 0; x < grad.cols; x++)
		grad.at<uchar>(y, x) = sqrt(pow(abs_grad_x.at<uchar>(y, x), 2) + pow(abs_grad_y.at<uchar>(y, x), 2));

	//to store the gradients grad_x.convertTo(grad_x,CV_32F);
	grad_x.convertTo(grad_x, CV_32FC1);
	grad_y.convertTo(grad_y, CV_32FC1);

	phase(grad_x, grad_y, angles, false);
}
void CannyDetection(Mat src, Mat &canny){
	Mat gray;
	blur(src, gray, Size(3, 3));
	Canny(gray, canny, 100, 150, 3);
}

void EMapConstruct(Mat img, Mat & edgeMap, Mat & angles){
	// Get edgeMap by Sobel
	edgeMap = Mat(img.rows, img.cols, CV_8UC1);
	angles = Mat::zeros(img.rows, img.cols, CV_32F);;
	SobelDetection(img, edgeMap, angles);
#if CANNY
	CannyDetection(img, edgeMap);
#endif

#if DISPLAY
	ShowImg("Orientation", angles);
	ShowImg("EdgeMap", edgeMap);
#endif

#if SAVE
	Mat orientation;
	normalize(angles, orientation, 0x00, 0xFF, cv::NORM_MINMAX, CV_8U);
	imwrite("Image/Orientation.jpg", orientation);
	imwrite("Image/EdgeMap.jpg", edgeMap);
#endif
}

void StrokesGeneration(const Mat img, Mat & canvas, const vector<pair <Point, float>> drawPoints, const Mat edgeMap, const Mat angles, float iteration){

	int num = 0;
	
	// Draw on canvas
	for (int i = 0; i < drawPoints.size(); i++){
		//float p = drawPoints[i].second / 255.0;
		//float sp = (double)rand() / (RAND_MAX);
		//int seed = rand() % int(SAMPLEFREQ * iteration);
		//if (seed % int(SAMPLEFREQ * iteration) - 1 == 1){
		//if (1){//sp < p){
			num += 1;
			int x = drawPoints[i].first.x;
			int y = drawPoints[i].first.y;
			Scalar color = img.at<Vec3b>(y, x);
			float angle = angles.at<float>(y, x) + PI / 2;
			Stroke stroke(color, Point2f(x, y), 10.0/iteration, angle, 10.0);
			stroke.drawOnCanvas(canvas, edgeMap);
#if SIMULATION
			imshow("Simulation", canvas);
			waitKey(10);
#endif
	}
	cout << "Number of actual draw points: " << num << endl;

#if DISPLAY
	ShowImg("Original", img);
	ShowImg("Canvas", canvas);
#endif
}