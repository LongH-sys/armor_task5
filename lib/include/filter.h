#pragma once
#include <opencv2/core.hpp>
#include <vector>

void findLights(const cv::Mat& mask,
                std::vector<cv::RotatedRect>& lights,
                std::vector<double>& angles);
