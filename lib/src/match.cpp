#include "match.h"

#include <opencv2/imgproc.hpp>

#include <cmath>

std::vector<cv::RotatedRect> matchLights(
    const std::vector<cv::RotatedRect>& lights,
    const std::vector<double>& angles)
{
    std::vector<cv::RotatedRect> armors;
    std::vector<bool> used(lights.size(), false);
    for (int i = 0;i < static_cast<int>(lights.size());i++)
    {
        if (used[i])
        {
            continue;
        }
        for (int j = i + 1;j < static_cast<int>(lights.size());j++)
        {
            if (used[j])
            {
                continue;
            }
            double length1 = std::fmax(lights[i].size.width, lights[i].size.height);
            double length2 = std::fmax(lights[j].size.width, lights[j].size.height);
            double lengthRate = std::fmax(length1, length2) / std::fmin(length1, length2);
            if (lengthRate > 2.0)
            {
                continue;
            }

            double angleDiff = std::abs(angles[i] - angles[j]);
            if (angleDiff > 20.0)
            {
                continue;
            }

            double averageLength = (length1 + length2) / 2.0;
            double heightDiff = std::abs(lights[i].center.y - lights[j].center.y);
            if (heightDiff / averageLength > 0.6)
            {
                continue;
            }

            cv::Point2f centerDiff = lights[i].center - lights[j].center;
            double distance = cv::norm(centerDiff);
            double distanceRate = distance / averageLength;
            if (distanceRate < 1.2 || distanceRate > 5.5)
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
            double armor_long_side = std::fmax(armorRect.size.width, armorRect.size.height);
            double armorArea = armorRect.size.area();
            if (armor_long_side > 200.0)
            {
                continue;
            }
            if (armorArea > 5000.0)
            {
                continue;
            }
            cv::Point2f armorVertices[4];
            armorRect.points(armorVertices);
            std::vector<cv::Point2f> armorContour;
            
            for (int k = 0; k < 4; k++)
            {
                armorContour.push_back(armorVertices[k]);
            }
            bool hasThirdLights = false;
            for (int k = 0; k < static_cast<int>(lights.size()); k++)
            {
                if (k == i || k == j)
                {
                    continue;
                }
                double position = cv::pointPolygonTest(armorContour, lights[k].center, false);
                if (position >= 0)
                {
                    hasThirdLights = true;
                    break;
                }
            }
            if (hasThirdLights)
            {
                continue;
            }
            armors.push_back(armorRect);
            used[i] = true;
            used[j] = true;
            break;
        }
    }

    return armors;
}

std::vector<cv::RotatedRect> filterArmors(
    const std::vector<cv::RotatedRect>& armors)
{
    std::vector<bool> removeArmor(armors.size(), false);
    for (int i =0; i < static_cast<int>(armors.size()); i++)
    {
        cv::Point2f outPoints[4];
        armors[i].points(outPoints);
        std::vector<cv::Point2f> outContour;
        for (int k = 0; k < 4; k++)
        {
            outContour.push_back(outPoints[k]);
        }

        for (int j = 0; j < static_cast<int>(armors.size()); j++)
        {
            if (i == j)
            {
                continue;
            }
            if (armors[i].size.area() <= armors[j].size.area())
            {
                continue;
            }
            cv::Point2f innerPoints[4];
            armors[j].points(innerPoints);
            bool allInside = true;
            for (int k = 0; k < 4; k++)
            {
                double position = cv::pointPolygonTest(outContour, innerPoints[k], false);
                if (position < 0)
                {
                    allInside = false;
                    break;
                }
                if (allInside)
                {
                    removeArmor[i] = true;
                    break;
                }
            }
        }
    }
    std::vector<cv::RotatedRect> result;
    for (int i = 0; i < armors.size(); i++)
    {
        if (!removeArmor[i])
        {
            result.push_back(armors[i]);
        }
    }
    return result;
}
