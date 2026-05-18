#pragma once
#include <opencv2/opencv.hpp>
#include "KeyProcessor.hpp"

class FrameProcessor {
public:
    int brightness;
    cv::Point mousePos;
    bool isDrawing;
    cv::Mat canvas;

    FrameProcessor();
    cv::Mat process(const cv::Mat& input, Mode mode);
    static void mouseCallback(int event, int x, int y, int flags, void* userdata);
};
