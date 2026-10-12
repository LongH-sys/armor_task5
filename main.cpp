#include <opencv2/core.hpp>

#include <opencv2/videoio.hpp>

#include <opencv2/highgui.hpp>

#include <opencv2/imgproc.hpp>

#include <iostream>

#include <vector>

int main()
{
    cv::Mat frame;
    cv::Mat frame_gaussian;
    cv::Mat frame_hsv;
    cv::Mat frame_mask;
    
    std::vector<std::vector<cv::Point>>contours;
    std::vector<cv::Vec4i> hierarchy;
    
    int hmin = 90;
    int hmax = 120;
    int smin = 200;
    int smax = 255;
    int vmin = 155;
    int vmax = 255;
    
    cv::createTrackbar("Hue min", "Trackbars", &hmin, 179);
    cv::createTrackbar("Hue max", "Trackbars", &hmax, 179);
    cv::createTrackbar("Sat min", "Trackbars", &smin, 255);
    cv::createTrackbar("Sat max", "Trackbars", &smax, 255);
    cv::createTrackbar("Val min", "Trackbars", &vmin, 255);
    cv::createTrackbar("Val max", "Trackbars", &vmax, 255);
    
    cv::VideoCapture cap("drone_blue3.avi");
    
    cv::namedWindow("Trackbars", cv::WINDOW_NORMAL);
    cv::resizeWindow("Trackbars", 640, 300);
    
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
        cv::drawContours(frame_result, contours, -1, cv::Scalar(0,255,0), 1);

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



    }
    
    cap.release();
    cv::destroyAllWindows();
    return 0;
}