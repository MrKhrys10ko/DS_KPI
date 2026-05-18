#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/dnn.hpp>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <atomic>
#include <string>

class FaceDetector {
private:
    cv::dnn::Net net;
    std::thread worker;
    std::mutex mtx;
    std::condition_variable cv;

    cv::Mat frameToProcess;
    std::vector<cv::Rect> lastDetections;

    std::atomic<bool> running;
    bool hasNewFrame;

    void detectLoop();

public:
    FaceDetector(const std::string& prototxt, const std::string& model);
    ~FaceDetector();

    // Для багатопотокового режиму (Рівень 2)
    void updateFrame(const cv::Mat& frame);
    std::vector<cv::Rect> getDetections();

    // Для однопотокового режиму з підвисанням (Рівень 1)
    std::vector<cv::Rect> detectSingleThread(const cv::Mat& frame);
};
