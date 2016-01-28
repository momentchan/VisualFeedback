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

void nonMaximaSuppression(const Mat& src, const int sz, Mat& dst, const Mat mask) {

	// initialise the block mask and destination
	const int M = src.rows;
	const int N = src.cols;
	const bool masked = !mask.empty();
	Mat block = 255 * Mat_<uint8_t>::ones(Size(2 * sz + 1, 2 * sz + 1));
	dst = Mat_<uint8_t>::zeros(src.size());

	// iterate over image blocks
	for (int m = 0; m < M; m += sz + 1) {
		for (int n = 0; n < N; n += sz + 1) {
			Point  ijmax;
			double vcmax, vnmax;

			// get the maximal candidate within the block
			Range ic(m, min(m + sz + 1, M));
			Range jc(n, min(n + sz + 1, N));
			minMaxLoc(src(ic, jc), NULL, &vcmax, NULL, &ijmax, masked ? mask(ic, jc) : noArray());
			Point cc = ijmax + Point(jc.start, ic.start);

			// search the neighbours centered around the candidate for the true maxima
			Range in(max(cc.y - sz, 0), min(cc.y + sz + 1, M));
			Range jn(max(cc.x - sz, 0), min(cc.x + sz + 1, N));

			// mask out the block whose maxima we already know
			Mat_<uint8_t> blockmask;
			block(Range(0, in.size()), Range(0, jn.size())).copyTo(blockmask);
			Range iis(ic.start - in.start, min(ic.start - in.start + sz + 1, in.size()));
			Range jis(jc.start - jn.start, min(jc.start - jn.start + sz + 1, jn.size()));
			blockmask(iis, jis) = Mat_<uint8_t>::zeros(Size(jis.size(), iis.size()));

			minMaxLoc(src(in, jn), NULL, &vnmax, NULL, &ijmax, masked ? mask(in, jn).mul(blockmask) : blockmask);
			Point cn = ijmax + Point(jn.start, in.start);

			// if the block centre is also the neighbour centre, then it's a local maxima
			if (vcmax > vnmax) {
				dst.at<uint8_t>(cc.y, cc.x) = 255;
			}
		}
	}
}


void colorDiffer(const Mat target, Mat detect, vector<pair <Point, float>> & drawPoints, float iteration) {
	//vector<pair <Point, float>> drawPoints;

	// rgb to cmyk
	Mat differImage = target.clone();
	Mat differMap = Mat(target.rows, target.cols, CV_8UC1);
	differMap.setTo(0);

	// Compute Difference 
	for (int y = 0; y < target.rows; y++) {
		for (int x = 0; x < target.cols; x++) {
			int targetCMYK[4];
			int detectCMYK[4];
			rgb2cmyk(target.at<Vec3b>(y, x), targetCMYK);
			rgb2cmyk(detect.at<Vec3b>(y, x), detectCMYK);
			ArrayXXf differ(1, 4);
			differ << abs(targetCMYK[0] - detectCMYK[0]), abs(targetCMYK[1] - detectCMYK[1]), abs(targetCMYK[2] - detectCMYK[2]), abs(targetCMYK[3] - detectCMYK[3]);
			float maxDiffer = differ.maxCoeff();
			differMap.at<uchar>(y, x) = (uchar)maxDiffer;
			if (maxDiffer > DRAWTHRESH / iteration){
				circle(differImage, Point(x, y), 0, Scalar(0, 0, maxDiffer));
				//drawPoints.push_back(make_pair(Point(x, y), maxDiffer));
			}
			else if (iteration == 1){
				detect.at<Vec3b>(y, x) = target.at<Vec3b>(y, x);
			}
		}
	}
	ShowImg("differMap", differMap);

	// Non maximun suppresion
	Mat mask = (differMap > DRAWTHRESH / iteration);	// only look for local maxima above the value of 1
	ShowImg("mask", mask);
	// find the local maxima with a window of 50
	Mat maxima;
	nonMaximaSuppression(differMap, 1, maxima, mask);
	// optionally set all non-maxima to zero
	differMap.setTo(0, maxima == 0);
	ShowImg("random", differMap);

	for (int y = 0; y < target.rows; y++) {
		for (int x = 0; x < target.cols; x++) {
			if ((float)differMap.at<uchar>(y, x) != 0.0)
				//cout << (float)differMap.at<uchar>(y, x) << endl;
				drawPoints.push_back(make_pair(Point(x, y), (float)differMap.at<uchar>(y, x)));
		}
	}
	sort(drawPoints.begin(), drawPoints.end(), ColorDifferenceCompare);

	cout << "Number of total draw points: " << drawPoints.size() << endl;
	ShowImg("", differImage);
}