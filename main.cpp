#include <opencv2/core.hpp>

#include <opencv2/videoio.hpp>

#include <opencv2/highgui.hpp>

#include <opencv2/imgproc.hpp>

#include <iostream>

#include <vector>

#include <math.h>
int main()
{
    cv::Mat frame;
    cv::Mat frame_gaussian;
    cv::Mat frame_hsv;
    cv::Mat frame_mask;

    int hmin = 90;
    int hmax = 120;
    int smin = 200;
    int smax = 255;
    int vmin = 155;
    int vmax = 255;

    std::vector<std::vector<cv::Point>>contours;
    std::vector<cv::Vec4i> hierarchy;

    cv::VideoCapture cap("drone_blue3.avi");
    bool paused = false;
    while(true)
    {
        if (!paused)
        {
            if (!cap.read(frame))
            {
                break;
            }
        }
        
        cv::GaussianBlur(frame, frame_gaussian, cv::Size(3,3), 0, 0);
        cv::cvtColor(frame_gaussian, frame_hsv, cv::COLOR_BGR2HSV);
        
        cv::Scalar lower(hmin, smin, vmin);
        cv::Scalar upper(hmax, smax, vmax);
        cv::inRange(frame_hsv, lower, upper, frame_mask);
        
        cv::Mat kernel = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3,3));
        cv::dilate(frame_mask, frame_mask, kernel); 
        cv::erode(frame_mask, frame_mask, kernel);
        
        cv::findContours(frame_mask, contours, hierarchy, cv::RETR_EXTERNAL, cv::CHAIN_APPROX_SIMPLE);
        cv::Mat frame_result = frame.clone();

        for(int i = 0;i < contours.size();i++)
        {
            double area = contourArea(contours[i]);
            if (area < 5.0 || area > 150.0)
            {
                continue;
            }
            
            cv::RotatedRect rotatedRect = cv::minAreaRect(contours[i]);
            
            double longSide = fmax(rotatedRect.size.width, rotatedRect.size.height);
            double shortSide = fmin(rotatedRect.size.width, rotatedRect.size.height);
            
            if (shortSide == 0)
            {
                continue;
            }
            double rate = longSide / shortSide;
            if (rate < 2.0 || rate > 10.0)
            {
                continue;
            }

            cv::Point2f vertices[4];
            rotatedRect.points(vertices);
            cv::Point2f edge1 = vertices[1] - vertices[0];
            cv::Point2f edge2 = vertices[2] - vertices[1];
            cv::Point2f longEdge = cv::norm(edge1) >= cv::norm(edge2) ? edge1 : edge2;
            double tilt = std::atan2(abs(longEdge.x), abs(longEdge.y)) * 180.0 / M_PI;

            if (tilt > 35.0)
            {
                continue;
            }

            for (int j = 0;j < 4;j++)
            {
                cv::line(frame_result, vertices[j], vertices[(j + 1) % 4], cv::Scalar(0,255,0), 1);
            }
        }
        
        cv::imshow("frame", frame);
        cv::imshow("frame_", frame_result);
        int key = cv::waitKey(1);
        
        if (key == ' ')
        {
            paused = !paused;
        }
        
        if (key == 'q')
        {
            break;
        }
        
        if (cv::getWindowProperty("frame", cv::WND_PROP_AUTOSIZE) < 0)
        {
            break;
        }
        
        if (key == 'd')
        {
            double fps = cap.get(cv::CAP_PROP_FPS);
            double pos = cap.get(cv::CAP_PROP_POS_FRAMES);
            double total = cap.get(cv::CAP_PROP_FRAME_COUNT);

            if (fps > 0 && total > 0)
            {
                double target = pos + fps * 5;

                if (target >= total)
                {
                    target = total - 1;
                }    
                if (cap.set(cv::CAP_PROP_POS_FRAMES, target))
                {
                    paused = false; 
                }   
            }
        }
        
        if (key == 'a')
        {
            double fps = cap.get(cv::CAP_PROP_FPS);
            double pos = cap.get(cv::CAP_PROP_POS_FRAMES);
            double total = cap.get(cv::CAP_PROP_FRAME_COUNT);

            if (fps > 0 && total > 0)
            {
                double target = pos - fps * 5;

                if (target <= 0)
                {
                    target = 1;
                }    
                if (cap.set(cv::CAP_PROP_POS_FRAMES, target))
                {
                    paused = false; 
                }   
            }
        }

    }
    
    cap.release();
    cv::destroyAllWindows();
    return 0;
}