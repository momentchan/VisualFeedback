#include "FuncDeclaration.h"
#include "DataStructure.h"

int main(int argc, const char** argv)
{	

	//// *********************
	////     Target color
	//// *********************
	//Mat colorPatch = imread("color_patch2.jpg");
	//imshow("targetRGB",targetRGB);
	//waitKey(0);

	//targetBGR = colorPatch.at<Vec3b>(0, 0);
	//rgb2cmyk(targetBGR, targetCMYK);
	

	// Read Data 
	//Mat targetImg = imread("colorImgAlexis.jpg");
	Mat targetImg = imread("apple.jpg");

	ColorSeparation(targetImg);
	Mat detectImg = imread("firstDraw.jpg");
	//Mat detectImg = imread("Image/Origin.jpg");
	//CaptureFrame(detectImg);


#if SAVE
//	imwrite("Image/Origin.jpg", detectImg);
#endif

	// Construct Energy Map
	Mat edgeMap, angles;
	EMapConstruct(targetImg, edgeMap, angles);

	for (int i = 0; i < ITERATION; i++){
		// Compute color difference
		vector<pair <Point, float>> drawPoints;
		colorDiffer(targetImg, detectImg, drawPoints, i+1);

		// Generate Strokes
		StrokesGeneration(targetImg, detectImg, drawPoints, edgeMap, angles, i+1);
#if SAVE
		string fileName = outputFileName("Image/Feedback", i+1, ".jpg");
		imwrite(fileName, detectImg);
#endif
	}
}
