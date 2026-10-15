#include "preprocess.h"

#include <opencv2/imgproc.hpp>

cv::Mat preprocess(const cv::Mat& frame)
{
    cv::Mat frame_gaussian, frame_hsv, frame_mask;
    const int hmin = 90, hmax = 120;
    const int smin = 200, smax = 255;
    const int vmin = 155, vmax = 255;

    cv::GaussianBlur(frame, frame_gaussian, cv::Size(3,3), 0, 0);
    cv::cvtColor(frame_gaussian, frame_hsv, cv::COLOR_BGR2HSV);

    cv::Scalar lower(hmin, smin, vmin);
    cv::Scalar upper(hmax, smax, vmax);
    cv::inRange(frame_hsv, lower, upper, frame_mask);

    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
    cv::dilate(frame_mask, frame_mask, kernel); 
    cv::erode(frame_mask, frame_mask, kernel);

    return frame_mask;
}
