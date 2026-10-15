#include "preprocess.h"

#include <opencv2/imgproc.hpp>

cv::Mat preprocess(const cv::Mat& frame, bool choice)
{
    cv::Mat frame_gaussian, frame_hsv, frame_mask;
    cv::GaussianBlur(frame, frame_gaussian, cv::Size(3,3), 0, 0);
    cv::cvtColor(frame_gaussian, frame_hsv, cv::COLOR_BGR2HSV);
    
    if (choice)
    {
        cv::Mat mask1, mask2;
        cv::inRange(frame_hsv, cv::Scalar(0, 70, 90), cv::Scalar(15, 255, 255), mask1);
        cv::inRange(frame_hsv, cv::Scalar(170, 120, 115), cv::Scalar(179, 255, 255), mask2);
        cv::bitwise_or(mask1, mask2, frame_mask);
    }
    else
    {
        cv::inRange(frame_hsv, cv::Scalar (90, 120, 115), cv::Scalar (120, 255, 255), frame_mask);
    }
    
    cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(5,5));
    cv::morphologyEx(frame_mask, frame_mask, cv::MORPH_CLOSE, kernel);
    return frame_mask;
}
