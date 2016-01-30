#include "FuncDeclaration.h"
#include "MeanShift.h"

void ColorSeparation(const Mat targetImg){
	
	// Mean shifting
	Mat colorSegment = Mat(targetImg.size(), CV_8UC3, Scalar(255, 255, 255));
	IplImage* img = cvCloneImage(&(IplImage)targetImg);
	int **ilabels = new int *[img->height];
	for (int i = 0; i < img->height; i++) ilabels[i] = new int[img->width];
	int regionNum = MeanShift(img, ilabels);
	cout << "Segment region number: " << regionNum << endl;


	vector <vector<Point>> separateRegionPoints;
	vector <Mat> separateRegions;
	vector <Mat> separateRegionsBlack;
	vector<Scalar> colorValue;

	// Initial
	for (int i = 0; i < regionNum; i++){
		colorValue.push_back(Scalar(0, 0, 0));
		separateRegions.push_back(Mat(targetImg.size(), CV_8UC3, Scalar(255, 255, 255)));
	}
	separateRegionPoints.resize(regionNum);

	// Sort blobs size
	for (int i = 0; i < targetImg.rows; i++)
	for (int j = 0; j < targetImg.cols; j++)
	{
		int label = ilabels[i][j];
		separateRegionPoints[label].push_back(Point(i, j));
	}
	sort(separateRegionPoints.begin(), separateRegionPoints.end(), CompareLength);

	// Compute average color
	for (int i = 0; i < separateRegionPoints.size(); i++){
		int pixNum = separateRegionPoints[i].size();
		for (int j = 0; j < separateRegionPoints[i].size(); j++)
		{
			int x = separateRegionPoints[i][j].x;
			int y = separateRegionPoints[i][j].y;
			colorValue[i] += Scalar(targetImg.at<Vec3b>(x, y)[0], targetImg.at<Vec3b>(x, y)[1], targetImg.at<Vec3b>(x, y)[2])/pixNum;
		}
	}

	// Recover origin average color
	for (int i = 0; i < separateRegionPoints.size(); i++){
		Scalar color = colorValue[i];
		for (int j = 0; j < separateRegionPoints[i].size(); j++){
			int x = separateRegionPoints[i][j].x;
			int y = separateRegionPoints[i][j].y;
			colorSegment.at<Vec3b>(x, y) = Vec3b(color[0], color[1], color[2]);
			separateRegions[i].at<Vec3b>(x, y) = Vec3b(color[0], color[1], color[2]);
		}
		//separateRegionsBlack.push_back(FindLargestRegion(separateRegions[i]));
	}
	// Background Removal
	colorValue.erase(colorValue.begin());
	separateRegions.erase(separateRegions.begin());
	//separateRegionsBlack.erase(separateRegionsBlack.begin());

	imshow("Color Segment", colorSegment);
	imwrite("Color Segment.jpg", colorSegment);
	waitKey(0);

}