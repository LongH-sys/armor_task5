#pragma once
#include <opencv2/core.hpp>
#include <vector>

std::vector<cv::RotatedRect> matchLights(
    const std::vector<cv::RotatedRect>& lights,
    const std::vector<double>& angles);
