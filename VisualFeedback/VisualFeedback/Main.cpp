#include "FuncDeclaration.h"
#include "DataStructure.h"

int main(int argc, const char** argv)
{	
	// Read Data
	Mat targetImg = imread("colorImgAlexis.jpg");
	Mat detectImg = imread("OriginColorSegment.jpg");

	// Construct Energy Map
	Mat edgeMap, angles;
	EMapConstruct(targetImg, edgeMap, angles);

	for (int i = 0; i < ITERATION; i++){
		// Compute color difference
		vector<Point> drawPoints;
		colorDiffer(targetImg, detectImg, drawPoints, i+1);

		// Generate Strokes
		StrokesGeneration(targetImg, detectImg, drawPoints, edgeMap, angles, i+1);
#if SAVE
		string fileName = outputFileName("Image/Feedback", i+1, ".jpg");
		imwrite(fileName, detectImg);
#endif
	}
}
