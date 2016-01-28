#include "main.h"

void FindBlobs(const Mat &, vector < vector<Point2i>> &, Mat &);
Mat FindLargestRegion(const Mat img);

void ColorSeparation();
void ColorDefinement();
bool CompareLength(vector<Point>, vector<Point>);


// Utility
string outputFileName(string file_name, int index, string type);
float BilinearInterplation(float x, float y);
void ShowImg(string window_name, Mat img);
bool ColorDifferenceCompare(pair <Point, float> c1, pair <Point, float> c2);

// Color Feedback
void colorDiffer(const Mat target, Mat detect, vector<pair <Point, float>> & drawPoints, float iteration);
void rgb2cmyk(const Vec3b bgr, int * cmyk);

// Strokes Generation
void EMapConstruct(Mat img, Mat & edgeMap, Mat & angles);
void SobelDetection(Mat src, Mat & grad, Mat & angles);
void CannyDetection(Mat src, Mat &canny);
void StrokesGeneration(const Mat img, Mat & canvas, const vector<pair <Point, float>> drawPoints, const Mat edgeMap, const Mat angles, float iteration);

// Camera Control
void CaptureFrame(Mat & detectImg);