#include "FuncDeclaration.h"
#include "MeanShift.h"
extern Mat targetImg;
Mat colorSegment;

vector <vector<Point>> separateRegionPoints;
vector <Mat> separateRegions;
vector <Mat> separateRegionsBlack;
vector <Mat> fillRegions;
vector<Scalar> colorValue;
vector<Scalar> colorPaletteRGB;
vector<Scalar> colorPaletteHSV;
vector<Scalar> colorPaletteTransform;
vector<vector<int>> colorIndexes;

int colorSpace = CV_BGR2Luv;
Mat origintargetImg;
vector<Scalar> colorTransformValue;
vector<Scalar> colorHSVValue;

void ColorSeparation(){
	
	// Bilateral Filtering
	Mat bilateralFilteredImg = targetImg;
	
	// Mean shifting
	colorSegment = Mat(targetImg.size(), CV_8UC3, Scalar(255, 255, 255));
	IplImage* img = cvCloneImage(&(IplImage)targetImg);
	int **ilabels = new int *[img->height];
	for (int i = 0; i < img->height; i++) ilabels[i] = new int[img->width];
	int regionNum = MeanShift(img, ilabels);
	cout << "Segent region number: " << regionNum << endl;

	Mat colorTransform;
	origintargetImg = Mat(targetImg.size(), CV_8UC3, Scalar(255, 255, 255));
	cvtColor(bilateralFilteredImg, colorTransform, colorSpace);

	Mat colorHSV;
	cvtColor(bilateralFilteredImg, colorHSV, CV_BGR2HSV);

	// Initial
	for (int i = 0; i < regionNum; i++){
		colorValue.push_back(Scalar(0, 0, 0));
		colorHSVValue.push_back(Scalar(0, 0, 0));
		colorTransformValue.push_back(Scalar(0, 0, 0));
		separateRegions.push_back(Mat(targetImg.size(), CV_8UC3, Scalar(255, 255, 255)));
	}
	separateRegionPoints.resize(regionNum);


	// Sort blobs size
	for (int i = 0; i < bilateralFilteredImg.rows; i++)
	for (int j = 0; j < bilateralFilteredImg.cols; j++)
	{
		int label = ilabels[i][j];
		separateRegionPoints[label].push_back(Point(i, j));
	}
	sort(separateRegionPoints.begin(), separateRegionPoints.end(), CompareLength);

	// Compute average color
	for (int i = 0; i < separateRegionPoints.size(); i++)
	for (int j = 0; j < separateRegionPoints[i].size(); j++)
	{
		int x = separateRegionPoints[i][j].x;
		int y = separateRegionPoints[i][j].y;
		colorValue[i] += Scalar(bilateralFilteredImg.at<Vec3b>(x, y)[0], bilateralFilteredImg.at<Vec3b>(x, y)[1], bilateralFilteredImg.at<Vec3b>(x, y)[2]);
		colorHSVValue[i] += Scalar(colorHSV.at<Vec3b>(x, y)[0], colorHSV.at<Vec3b>(x, y)[1], colorHSV.at<Vec3b>(x, y)[2]);
		colorTransformValue[i] += Scalar(colorTransform.at<Vec3b>(x, y)[0], colorTransform.at<Vec3b>(x, y)[1], colorTransform.at<Vec3b>(x, y)[2]);
	}


	for (int i = 0; i < regionNum; i++){
		int pixNum = separateRegionPoints[i].size();
		colorValue[i] /= pixNum;
		colorTransformValue[i] /= pixNum;
		colorHSVValue[i] /= pixNum;
		//cout << colorValue[i] << endl;
	}


	// Recover origin average color
	for (int i = 0; i < separateRegionPoints.size(); i++){
		Scalar color = colorValue[i];
		for (int j = 0; j < separateRegionPoints[i].size(); j++){
			int x = separateRegionPoints[i][j].x;
			int y = separateRegionPoints[i][j].y;
			origintargetImg.at<Vec3b>(x, y) = Vec3b(color[0], color[1], color[2]);
			separateRegions[i].at<Vec3b>(x, y) = Vec3b(color[0], color[1], color[2]);
		}
		separateRegionsBlack.push_back(FindLargestRegion(separateRegions[i]));
	}
	// Background Removal
	colorValue.erase(colorValue.begin());
	colorTransformValue.erase(colorTransformValue.begin());
	colorHSVValue.erase(colorHSVValue.begin());
	separateRegions.erase(separateRegions.begin());
	separateRegionsBlack.erase(separateRegionsBlack.begin());

	imshow("Origin Color Image", origintargetImg);
	imwrite("OriginColorSegment2.jpg", origintargetImg);
	waitKey(0);

}


