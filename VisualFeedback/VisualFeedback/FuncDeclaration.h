#pragma once
#include "main.h"

// Color Separation
void ColorSeparation(const Mat targetImg, vector<StrokeCluster> &fisrtDrawCluster);
bool CompareLength(vector<Point>, vector<Point>);

// Color Feedback
void colorDiffer(const Mat target, Mat detect, vector<pair <Point, float>> & drawPoints, float iteration);

// Strokes Generation
void EMapConstruct(Mat img, Mat & edgeMap, Mat & angles);
void SobelDetection(Mat src, Mat & grad, Mat & angles);
void CannyDetection(Mat src, Mat &canny);
void StrokesGeneration(const Mat img, Mat & canvas, const vector<pair <Point, float>> drawPoints, const Mat edgeMap, const Mat angles, float iteration);

// Camera Control
void CaptureFrame(VideoCapture captureDevice, Mat detectImg);

// Utility
string outputFileName(string file_name, int index, string type);
float BilinearInterplation(float x, float y);
void ShowImg(string window_name, Mat img, int time = 0);
bool ColorDifferenceCompare(pair <Point, float> c1, pair <Point, float> c2);
void rgb2cmyk(const Vec3b bgr, Vec4f & cmyk);
