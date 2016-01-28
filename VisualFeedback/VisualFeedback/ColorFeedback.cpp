#include "FuncDeclaration.h"

void rgb2cmyk(const Vec3b bgr, int * cmyk) {
	int B = bgr.val[0];
	int G = bgr.val[1];
	int R = bgr.val[2];
	float r = R / 255.;
	float g = G / 255.;
	float b = B / 255.;
	float k = std::min(std::min(1 - r, 1 - g), 1 - b);

	cmyk[0] = (1 - r - k) / (1 - k) * 255.;
	cmyk[1] = (1 - g - k) / (1 - k) * 255.;
	cmyk[2] = (1 - b - k) / (1 - k) * 255.;
	cmyk[3] = k * 255.;
}

void colorDiffer(const Mat target, Mat detect, vector<Point> & drawPoints, float iteration) {

	// rgb to cmyk
	Mat differMap = target.clone();
	for (int y = 0; y < target.rows; y++) {
		for (int x = 0; x < target.cols; x++) {
			int targetCMYK[4];
			int detectCMYK[4];
			rgb2cmyk(target.at<Vec3b>(y, x), targetCMYK);
			rgb2cmyk(detect.at<Vec3b>(y, x), detectCMYK);
			ArrayXXf differ(1, 4);
			differ << abs(targetCMYK[0] - detectCMYK[0]), abs(targetCMYK[1] - detectCMYK[1]), abs(targetCMYK[2] - detectCMYK[2]), abs(targetCMYK[3] - detectCMYK[3]);
			if (differ.maxCoeff() > DRAWTHRESH / iteration){
				circle(differMap, Point(x, y), 0, Scalar(0, 0, differ.maxCoeff()));
				drawPoints.push_back(Point(x, y));
			}
			else if (iteration == 1){
				detect.at<Vec3b>(y, x) = target.at<Vec3b>(y, x);
			}
		}
	}
	cout << "Number of total draw points: " << drawPoints.size() << endl;
	ShowImg("", differMap);

}