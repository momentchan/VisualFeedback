#include "FuncDeclaration.h"

int main(int argc, const char** argv)
{	
	// Read Data 
	Mat targetImg = imread("apple.jpg");

	// *********************
	//  First Layer Drawing
	// *********************
#if MEANSHIFT
	vector<StrokeCluster> fisrtDrawCluster;
	ColorSeparation(targetImg, fisrtDrawCluster);
	Mat detectImg = Mat(targetImg.size(), CV_8UC3, Scalar(255, 255, 255));
	for (int i = 0; i < fisrtDrawCluster.size(); i++){
		int strokeNum = fisrtDrawCluster[i].getNum();
		for (int s = 0; s < strokeNum; s++){
			fisrtDrawCluster[i].getStroke(s).drawOnCanvas(detectImg);
			ShowImg("Simulation", detectImg,10);
		}
	}
	waitKey(0);
	destroyAllWindows();
	imwrite("firstDraw.jpg", detectImg);
#else
	Mat detectImg = imread("firstDraw.jpg");
#endif
	//Mat detectImg = imread("Image/Origin.jpg");
	//CaptureFrame(detectImg);


	// Construct Energy Map
	Mat edgeMap, angles;
	EMapConstruct(targetImg, edgeMap, angles);

	for (int i = 0; i < ITERATION; i++){
		// Compute color difference
		vector<pair <Point, float>> drawPoints;
		colorDiffer(targetImg, detectImg, drawPoints, i+1);

		// Generate Strokes
		StrokesGeneration(targetImg, detectImg, drawPoints, edgeMap, angles, i+1);
		string fileName = outputFileName("Image/Feedback", i+1, ".jpg");
		imwrite(fileName, detectImg);
	}
}
