#include "FuncDeclaration.h"
#include "DataStructure.h"
bool ColorDifferenceCompare(pair <Point, float> c1, pair <Point, float> c2) {
	float i = c1.second;
	float j = c2.second;
	return (i > j);
}

bool CompareLength(vector<Point> contour1, vector<Point> contour2) {
	double i = arcLength(contour1, false);
	double j = arcLength(contour2, false);
	return (i > j);
}
bool CompareArea(vector<Point> area1, vector<Point> area2) {
	double i = area1.size();
	double j = area2.size();
	return (i > j);
}
bool CompareBox(Rect area1, Rect area2) {
	double i = area1.area();
	double j = area2.area();
	return (i > j);
}
int HistogramCalulation(Mat image, Mat & histImage, float alpha){
	// allcoate memory for no of pixels for each intensity value
	int histogram[256];

	// initialize all intensity values to 0
	for (int i = 0; i < 255; i++)
		histogram[i] = 0;

	// calculate the no of pixels for each intensity values
	for (int y = 0; y < image.rows; y++)
	for (int x = 0; x < image.cols; x++)
		histogram[(int)image.at<uchar>(y, x)]++;

	// draw the histograms
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / 256);

	histImage = Mat(hist_h, hist_w, CV_8UC1, Scalar(255, 255, 255));

	// find the maximum intensity element from histogram
	int max_num = histogram[0];
	int max = 0;
	int total_num = image.size().area();
	int threshold = total_num*alpha;
	int accumulate = 0;
	int T;
	bool find = false;

	for (int i = 1; i < 256; i++){
		if (!find){
			if (accumulate < threshold){
				accumulate += histogram[i];
			}
			else {
				find = true;
				T = i - 1;
				accumulate -= histogram[i - 1];
			}
		}
		if (max_num < histogram[i]){
			max_num = histogram[i];
			max = i;
		}
	}
	//cout << threshold << endl << T << endl << accumulate << endl << endl;

	// normalize the histogram between 0 and histImage.rows
	for (int i = 0; i < 255; i++){
		histogram[i] = ((double)histogram[i] / max_num)*histImage.rows;
	}

	// draw the intensity line for histogram
	for (int i = 0; i < 255; i++)
		line(histImage, Point(bin_w*(i), hist_h), Point(bin_w*(i), hist_h - histogram[i]), Scalar(0, 0, 0), 1, 8, 0);
	imwrite("b2_histogram.jpg", histImage);

	waitKey();
	return T;
}
Mat HoleFilling(Mat fill_region)
{
	//cvtColor(fill_region, fill_region, CV_BGR2GRAY);
	Mat image_thresh;
	threshold(fill_region, image_thresh, 125, 255, THRESH_BINARY);

	// Loop through the border pixels and if they're black, floodFill from there
	Mat mask;
	image_thresh.copyTo(mask);
	for (int i = 0; i < mask.cols; i++) {
		if (mask.at<char>(0, i) == 0)
			floodFill(mask, Point(i, 0), 255, 0, 10, 10);
		if (mask.at<char>(mask.rows - 1, i) == 0)
			floodFill(mask, Point(i, mask.rows - 1), 255, 0, 10, 10);
	}
	for (int i = 0; i < mask.rows; i++) {
		if (mask.at<char>(i, 0) == 0)
			floodFill(mask, Point(0, i), 255, 0, 10, 10);
		if (mask.at<char>(i, mask.cols - 1) == 0)
			floodFill(mask, Point(mask.cols - 1, i), 255, 0, 10, 10);
	}
	Mat holefilled;
	// Compare mask with original.
	image_thresh.copyTo(holefilled);
	for (int row = 0; row < mask.rows; ++row) {
		for (int col = 0; col < mask.cols; ++col) {
			if (mask.at<char>(row, col) == 0) {
				holefilled.at<char>(row, col) = 255;
			}
		}
	}
	bitwise_not(holefilled, holefilled);
	//imshow("Final image", holefilled);
	return holefilled;
	//imwrite("final.jpg", newImage);
	//waitKey(0);
}
void ConnectedComponent(const Mat img, vector<vector<Point2i>>& blobs){
	Mat binary;
	Mat labelImage;
	threshold(~img, binary, 0.0, 1.0, THRESH_BINARY);
	FindBlobs(binary, blobs, labelImage);
	//imwrite("label.jpg", labelImage);
	sort(blobs.begin(), blobs.end(), CompareLength);
}

Mat FindLargestRegion(const Mat img){
	Mat grayImg;
	cvtColor(img, grayImg, CV_BGR2GRAY);
	Mat binaryImg = grayImg < 250;
	threshold(binaryImg, binaryImg, 0.0, 1.0, THRESH_BINARY);

	Mat labelImage;
	vector<vector<Point2i>> blobs;
	FindBlobs(binaryImg, blobs, labelImage);
	sort(blobs.begin(), blobs.end(), CompareLength);

	Mat largestArea = Mat(img.size(), CV_8UC1);
	largestArea.setTo(0);
	for (int i = 0; i < blobs[0].size(); i++){
		int x = blobs[0][i].x;
		int y = blobs[0][i].y;
		largestArea.at<uchar>(y, x) = 255;
	}
	//imshow("", largestArea); waitKey(0);
	return largestArea;
}

void imageRefinement(Mat & src){
	int morph_elem = 0;
	int morph_size = 2;
	int open_operator = 2;
	int close_operator = 3;

	Mat element = getStructuringElement(morph_elem, Size(2 * morph_size + 1, 2 * morph_size + 1), Point(morph_size, morph_size));
	// Apply the specified morphology operation
	morphologyEx(src, src, close_operator, element);
	//morphologyEx(src, src, open_operator, element);
	//morphologyEx(src, src, open_operator, element);
	imshow("Refinement", src); waitKey(0);
	destroyWindow("Refinement");
}

int ConnectedComponentNumber(const Mat region1, const Mat region2){
	
	Mat binary1,binary2,binary;
	
	threshold(region1, binary1, 0.0, 1.0, THRESH_BINARY);
	threshold(region2, binary2, 0.0, 1.0, THRESH_BINARY);

	binary = binary1 + binary2;
	Mat labelImage;
	vector<vector<Point2i>> blobs;
	FindBlobs(binary, blobs, labelImage);
	//imshow("", region1 + region2); waitKey(0);
	
	return blobs.size();
}
void FindBlobs(const Mat &binary, vector < vector<Point2i> > &blobs, Mat &labelImage)
{
	blobs.clear();

	// Fill the labelImage with the blobs
	// 0  - background
	// 1  - unlabelled foreground
	// 2+ - labelled foreground

	binary.convertTo(labelImage, CV_32SC1);

	int label_count = 2; // starts at 2 because 0,1 are used already

	for (int y = 0; y < labelImage.rows; y++) {
		int *row = (int*)labelImage.ptr(y);
		for (int x = 0; x < labelImage.cols; x++) {
			if (row[x] != 1) {
				continue;
			}

			Rect rect;
			floodFill(labelImage, Point(x, y), label_count, &rect, 0, 0, 4);

			vector <Point2i> blob;

			for (int i = rect.y; i < (rect.y + rect.height); i++) {
				int *row2 = (int*)labelImage.ptr(i);
				for (int j = rect.x; j < (rect.x + rect.width); j++) {
					if (row2[j] != label_count) {
						continue;
					}
					blob.push_back(Point2i(j, i));
				}
			}
			blobs.push_back(blob);
			label_count++;
		}
	}
}

void ShowImg(string window_name, Mat img){
	imshow(window_name, img);
	waitKey(0);
}
string int2str(int &i) {
	string s;
	stringstream ss(s);
	ss << i;
	return ss.str();
}
string outputFileName(string file_name, int index, string type){
	file_name.append(int2str(index));
	file_name.append(type);
	return file_name;
}
void rgb2cmyk(const Vec3b bgr, Vec4f & cmyk) {
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
