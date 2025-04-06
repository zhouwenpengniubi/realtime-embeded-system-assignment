#include <opencv2/opencv.hpp>
#include <iostream>

int main()
{
    cv::VideoCapture cap(0); // Open camera0

    // Reduce resolution for better performance
    cap.set(cv::CAP_PROP_FRAME_WIDTH, 320);
    cap.set(cv::CAP_PROP_FRAME_HEIGHT, 240);

    if (!cap.isOpened())
    {
        std::cerr << "Cannot access the camera" << std::endl;
        return -1;
    }

    const std::string windowName = "Camera";
    cv::namedWindow(windowName, cv::WINDOW_AUTOSIZE);

    while (true)
    {
        cv::Mat frame;
        cap >> frame;

        if (frame.empty())
        {
            std::cerr << "Cannot read frame" << std::endl;
            break;
        }

        cv::imshow(windowName, frame);

        // Update the window
        cv::waitKey(1);

        // Exit if user clicks the window close button (getWindowProperty returns -1 if the window is closed)
        if (cv::getWindowProperty(windowName, cv::WND_PROP_VISIBLE) < 1)
        {
            break;
        }
    }

    cap.release();
    cv::destroyAllWindows();
    return 0;
}