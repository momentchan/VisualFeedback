#include "Main.h"


class Stroke{
public:
	Stroke(Scalar, Point2f, float, float, float);
	void findEndpoint(const float, const float, const Mat edgeMap);
	float BilinearInterplation(float x, float y, const Mat edgeMap);
	void drawOnCanvas(Mat & canvas, const Mat edgeMap){
		findEndpoint(float(canvas.cols), float(canvas.rows), edgeMap);
		line(canvas, start, end, color, radius);
	}
private:
	Scalar color;
	Point2f center, start, end;
	float radius, dir_x, dir_y, length;
};
