#include "filter.h"

#include <opencv2/imgproc.hpp>

#include <cmath>

void findLights(const cv::Mat& mask,
                std::vector<cv::RotatedRect>& lights,
                std::vector<double>& angles)
{
    lights.clear();
    angles.clear();
    std::vector<std::vector<cv::Point>> contours;
    std::vector<cv::Vec4i> hierarchy;
    cv::findContours(mask, contours, hierarchy,
                     cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);

    for(int i = 0;i < static_cast<int>(contours.size());i++)
    {
        double area = cv::contourArea(contours[i]);
        if (area < 2.0 || area > 400.0)
        {
            continue;
        }

        cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);

        double longSide = std::fmax(rotatedRect.size.width, rotatedRect.size.height);
        double shortSide = std::fmin(rotatedRect.size.width, rotatedRect.size.height);

        if (shortSide == 0 || longSide < 4.0)
        {
            continue;
        }
        double rate = longSide / shortSide;
        if (rate < 1.5 || rate > 10.0)
        {
            continue;
        }

        cv::Point2f vertices[4];
        rotatedRect.points(vertices);
        cv::Point2f edge1 = vertices[1] - vertices[0];
        cv::Point2f edge2 = vertices[2] - vertices[1];
        cv::Point2f longEdge = cv::norm(edge1) >= cv::norm(edge2) ? edge1 : edge2;
        double tilt = std::atan2(std::abs(longEdge.x), std::abs(longEdge.y)) * 180.0 / CV_PI;

        if (tilt > 37.0)
        {
            continue;
        }

        if (longEdge.y < 0)
        {
            longEdge = -longEdge;
        }
        double angle = std::atan2(longEdge.x, longEdge.y) * 180.0 / M_PI;
        lights.push_back(rotatedRect);
        angles.push_back(angle);
    }

}
