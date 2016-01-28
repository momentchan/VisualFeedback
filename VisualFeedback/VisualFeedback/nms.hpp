#ifndef NMS_HPP_
#define NMS_HPP_
#include <opencv2/core/core.hpp>
void nonMaximaSuppression(const cv::Mat& src, const int sz, cv::Mat& dst, const cv::Mat mask=cv::Mat());


#endif /* NMS_HPP_ */