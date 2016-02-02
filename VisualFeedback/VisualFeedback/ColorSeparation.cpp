#include "FuncDeclaration.h"
#include "MeanShift.h"
#include "DataStructure.h"
#define FIRSTDRAW 0
extern vector<StrokeCluster> fisrtDrawCluster;
int fillLines = 0;
bool turn = false;

void FindDrawPoints(int y, int x, int ys, int xe, Mat fill_region, ofstream & outputFile, float size, Vec3b rgb, Vec4f cmyk, int lineWidth, StrokeCluster &cluster){
	vector<Point> points;
	bool cross = true;

	//Find draw points
	while (y > ys && x < xe){		// Not touch boundary
		{
			if ((int)fill_region.at<uchar>(y, x) > 128 && !cross){
				points.push_back(Point(x - 1, y + 1));
				cross = true;
			}
			else if ((int)fill_region.at<uchar>(y, x) < 128 && cross){
				points.push_back(Point(x, y));
				cross = false;
			}
		}
		y = y - 1;
		x = x + 1;
	}
	if (points.size() % 2 != 0)  // add the boundary point
		points.push_back(Point(x, y));

	
	//Draw points
	int num = points.size() / 2;
	if (num > 0)
	if (!turn){
		for (int i = 0; i < num; i++)
		{
			cluster.addStroke(Stroke(rgb, cmyk, points[2 * i], points[2 * i + 1], lineWidth));
			outputFile << points[2 * i].x << " " << points[2 * i].y << " " << points[2 * i + 1].x << " " << points[2 * i + 1].y << endl;
			fillLines++;
		}
	}
	else {
		for (int i = num - 1; i >= 0; i--)
		if (norm(points[2 * i] - points[2 * i + 1]) > size*0.05)
		{
			cluster.addStroke(Stroke(rgb, cmyk, points[2 * i + 1], points[2 * i], lineWidth));
			outputFile << points[2 * i + 1].x << " " << points[2 * i + 1].y << " " << points[2 * i].x << " " << points[2 * i].y << endl;
			fillLines++;
		}
	}
	turn = !turn;
}
void FillSimulation(vector <Mat> fillRegions, vector<Scalar> colorValue, vector<StrokeCluster> &fisrtDrawCluster){
	float size = 0;
	int gap = 9;
	int lineWidth = 5;
	
	//Filling regions
	for (int i = 0; i < fillRegions.size(); i++) {
		// Boundary initialization
		int ys = 1;
		int ye = fillRegions[i].rows - 1;
		int xs = 1;
		int xe = fillRegions[i].cols - 1;
		Mat fillRgionBlack;
		cvtColor(fillRegions[i], fillRgionBlack, CV_RGB2GRAY);
		fillRgionBlack = fillRgionBlack > 245;
		ofstream outputFile;
		string fileName = outputFileName("drawPoints/fill", i, ".txt");
		outputFile.open(fileName);
		Point previousPoint;
		Vec3b fillColor = Vec3b(colorValue[i][0], colorValue[i][1], colorValue[i][2]);
		Vec4f cmyk;
		rgb2cmyk(fillColor, cmyk);
		// Write indexing of color
		outputFile << (int)fillColor[0] << " " << (int)fillColor[1] << " " << (int)fillColor[2] << endl;
		outputFile << (float)cmyk[0] << " " << (float)cmyk[1] << " " << (float)cmyk[2] <<" " << (float)cmyk[3] << endl;

		// Find draw points
		for (int j = ys; j <= ye; j = j + gap)
			FindDrawPoints(j, xs, ys, xe, fillRgionBlack, outputFile, size, fillColor, cmyk, lineWidth, fisrtDrawCluster[i]);
		// Last Row
		for (int k = xs; k <= xe; k = k + gap)
			FindDrawPoints(ye, k, ys, xe, fillRgionBlack, outputFile, size, fillColor, cmyk, lineWidth, fisrtDrawCluster[i]);
		outputFile.close();
	}
	cout << "\nTotal Number of fill lines: " << fillLines << endl << endl;
}
void ColorSeparation(const Mat targetImg){
	
	// Mean shifting
	Mat colorSegment = Mat(targetImg.size(), CV_8UC3, Scalar(255, 255, 255));
	IplImage* img = cvCloneImage(&(IplImage)targetImg);
	int **ilabels = new int *[img->height];
	for (int i = 0; i < img->height; i++) ilabels[i] = new int[img->width];
	int regionNum = MeanShift(img, ilabels);
	cout << "Segment region number: " << regionNum << endl;

	vector <vector<Point>> fillRegionPoints;
	vector <Mat> fillRegions;
	vector<Scalar> colorValue;

	// Initial
	for (int i = 0; i < regionNum; i++){
		colorValue.push_back(Scalar(0, 0, 0));
		fillRegions.push_back(Mat(targetImg.size(), CV_8UC3, Scalar(255, 255, 255)));
	}
	fillRegionPoints.resize(regionNum);

	// Sort blobs size
	for (int i = 0; i < targetImg.rows; i++)
	for (int j = 0; j < targetImg.cols; j++)
	{
		int label = ilabels[i][j];
		fillRegionPoints[label].push_back(Point(i, j));
	}
	sort(fillRegionPoints.begin(), fillRegionPoints.end(), CompareLength);

	// Compute average color
	for (int i = 0; i < fillRegionPoints.size(); i++){
		int pixNum = fillRegionPoints[i].size();
		for (int j = 0; j < fillRegionPoints[i].size(); j++)
		{
			int x = fillRegionPoints[i][j].x;
			int y = fillRegionPoints[i][j].y;
			colorValue[i] += Scalar(targetImg.at<Vec3b>(x, y)[0], targetImg.at<Vec3b>(x, y)[1], targetImg.at<Vec3b>(x, y)[2])/pixNum;
		}
	}
	// Recover origin average color
	for (int i = 0; i < fillRegionPoints.size(); i++){
		Scalar color = colorValue[i];
		for (int j = 0; j < fillRegionPoints[i].size(); j++){
			int x = fillRegionPoints[i][j].x;
			int y = fillRegionPoints[i][j].y;
			colorSegment.at<Vec3b>(x, y) = Vec3b(color[0], color[1], color[2]);
			fillRegions[i].at<Vec3b>(x, y) = Vec3b(color[0], color[1], color[2]);
		}
	}

	// Background Removal
	colorValue.erase(colorValue.begin());
	fillRegions.erase(fillRegions.begin());

	ShowImg("Color Segment", colorSegment,-1);
	imwrite("Color Segment.jpg", colorSegment);
	fisrtDrawCluster.resize(fillRegions.size());
	FillSimulation(fillRegions, colorValue, fisrtDrawCluster);
}