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


// Color Feedback
void colorDiffer(const Mat target, const Mat detect, vector<Point> & drawPoints, float iteration);


// Strokes Generation
void EMapConstruct(Mat img, Mat & edgeMap, Mat & angles);
void SobelDetection(Mat src, Mat & grad, Mat & angles);
void CannyDetection(Mat src, Mat &canny);
void StrokesGeneration(const Mat img, Mat & canvas, const vector<Point> drawPoints, const Mat edgeMap, const Mat angles, float iteration);