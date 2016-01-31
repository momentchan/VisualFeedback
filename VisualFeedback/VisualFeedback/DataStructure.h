#include "FuncDeclaration.h"

class Stroke{
public:
	Stroke(){}
	Stroke(Scalar, Point2f, float, float, float);
	void findEndpoint(const float, const float, const Mat edgeMap);
	float BilinearInterplation(float x, float y, const Mat edgeMap);
	void drawOnCanvas(Mat & canvas, const Mat edgeMap){
		findEndpoint(float(canvas.cols), float(canvas.rows), edgeMap);
		line(canvas, start, end, color, radius);
	}
	Scalar getColor(){ return color; }
private:
	Scalar color;
	Point2f center, start, end;
	float radius, dir_x, dir_y, length;
};

class StrokeCluster{
public:
	StrokeCluster(){ avgCMYK = Vec4f(0, 0, 0, 0); pointNum = 0;  maxInfo = make_pair(0, 0); }
	void addStroke(Stroke drawStroke);
	float computeDiffer(Scalar color);
	Stroke getStroke(int index){ return drawStrokes[index]; }
	int getNum(){ return pointNum; }
	Vec4f getColor(){ return avgCMYK; }
	void showMaxInfo(){ cout << maxInfo.first << " " << maxInfo.second << endl; }
	pair<int, double> getMaxInfo(){ return maxInfo; }
private:
	vector<Stroke> drawStrokes;
	Vec4f avgCMYK;
	int pointNum;
	pair<int, double> maxInfo;
};
