#include "preprocess.h"

#include "filter.h"

#include "match.h"

#include <opencv2/videoio.hpp>

#include <opencv2/highgui.hpp>

#include <opencv2/imgproc.hpp>

#include <iostream>

int main()
{
    cv::VideoCapture cap("drone_blue3.avi");
    if (!cap.isOpened())
    {
        std::cerr << "无法打开 drone_blue3.avi,请从 task5 项目根目录运行。" << std::endl;
        return 1;
    }

    cv::Mat frame;
    bool paused = false;
    while (true)
    {
        if (!paused && !cap.read(frame))
        {
            break;
        }

        cv::Mat frame_mask = preprocess(frame);
        std::vector<cv::RotatedRect> lights;
        std::vector<double> angles;
        findLights(frame_mask, lights, angles);
        std::vector<cv::RotatedRect> armors = matchLights(lights, angles);

        cv::Mat frame_result = frame.clone();
        for (int i = 0; i < static_cast<int>(armors.size()); i++)
        {
            cv::Point2f vertices[4];
            armors[i].points(vertices);
            for (int k = 0; k < 4; k++)
            {
                cv::line(frame_result, vertices[k], vertices[(k + 1) % 4],
                cv::Scalar(0, 0, 255), 2);
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
