#include "match.h"

#include <opencv2/imgproc.hpp>

#include <cmath>

std::vector<cv::RotatedRect> matchLights(
    const std::vector<cv::RotatedRect>& lights,
    const std::vector<double>& angles)
{
    std::vector<cv::RotatedRect> armors;

    for (int i = 0;i < static_cast<int>(lights.size());i++)
    {
        for (int j = i + 1;j < static_cast<int>(lights.size());j++)
        {
            double length1 = std::fmax(lights[i].size.width, lights[i].size.height);
            double length2 = std::fmax(lights[j].size.width, lights[j].size.height);
            double lengthRate = std::fmax(length1, length2) / std::fmin(length1, length2);
            if (lengthRate > 1.5)
            {
                continue;
            }

            double angleDiff = std::abs(angles[i] - angles[j]);
            if (angleDiff > 15.0)
            {
                continue;
            }

            double averageLength = (length1 + length2) / 2.0;
            double heightDiff = std::abs(lights[i].center.y - lights[j].center.y);
            if (heightDiff / averageLength > 0.5)
            {
                continue;
            }

            cv::Point2f centerDiff = lights[i].center - lights[j].center;
            double distance = cv::norm(centerDiff);
            double distanceRate = distance / averageLength;
            if (distanceRate < 1.8 || distanceRate > 5.0)
            {
                continue;
            }
            cv::Point2f points1[4];
            cv::Point2f points2[4];
            lights[i].points(points1);
            lights[j].points(points2);
            std::vector<cv::Point2f> armorPoints;
            for (int k = 0;k < 4;k++)
            {
                armorPoints.push_back(points1[k]);
                armorPoints.push_back(points2[k]);
            }
            cv::RotatedRect armorRect = cv::minAreaRect(armorPoints);
            armors.push_back(armorRect);
        }
    }

    return armors;
}
