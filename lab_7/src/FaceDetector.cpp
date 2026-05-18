#include "FaceDetector.hpp"
#include <iostream>
#include <chrono>

FaceDetector::FaceDetector(const std::string& prototxt, const std::string& model) : running(true), hasNewFrame(false) {
    try {
        net = cv::dnn::readNetFromCaffe(prototxt, model);
        std::cout << "[INFO] Модель DNN успішно завантажена." << std::endl;
    } catch (const cv::Exception& e) {
        std::cerr << "Помилка завантаження моделі: " << e.what() << std::endl;
    }
    // Запускаємо фоновий робочий потік
    worker = std::thread(&FaceDetector::detectLoop, this);
}

FaceDetector::~FaceDetector() {
    running = false;
    cv.notify_one();
    if (worker.joinable()) worker.join();
}

void FaceDetector::updateFrame(const cv::Mat& frame) {
    std::lock_guard<std::mutex> lock(mtx);
    frameToProcess = frame.clone();
    hasNewFrame = true;
    cv.notify_one(); // Будимо фоновий потік
}

std::vector<cv::Rect> FaceDetector::getDetections() {
    std::lock_guard<std::mutex> lock(mtx);
    return lastDetections;
}

std::vector<cv::Rect> FaceDetector::detectSingleThread(const cv::Mat& frame) {
    if (frame.empty() || net.empty()) return {};

    // Штучне навантаження за ТЗ, щоб показати викладачу просадку FPS
    std::this_thread::sleep_for(std::chrono::milliseconds(500));

    cv::Mat blob = cv::dnn::blobFromImage(frame, 1.0, cv::Size(300, 300), cv::Scalar(104.0, 177.0, 123.0));
    net.setInput(blob);
    cv::Mat detection = net.forward();

    cv::Mat detectionMat(detection.size[2], detection.size[3], CV_32F, detection.ptr<float>());
    std::vector<cv::Rect> currentDetections;

    for (int i = 0; i < detectionMat.rows; i++) {
        float confidence = detectionMat.at<float>(i, 2);
        if (confidence > 0.5) {
            int x1 = static_cast<int>(detectionMat.at<float>(i, 3) * frame.cols);
            int y1 = static_cast<int>(detectionMat.at<float>(i, 4) * frame.rows);
            int x2 = static_cast<int>(detectionMat.at<float>(i, 5) * frame.cols);
            int y2 = static_cast<int>(detectionMat.at<float>(i, 6) * frame.rows);
            currentDetections.push_back(cv::Rect(cv::Point(x1, y1), cv::Point(x2, y2)));
        }
    }
    return currentDetections;
}

void FaceDetector::detectLoop() {
    while (running) {
        cv::Mat frame;
        {
            std::unique_lock<std::mutex> lock(mtx);
            cv.wait(lock, [this] { return hasNewFrame || !running; });
            if (!running) break;
            frame = frameToProcess.clone();
            hasNewFrame = false;
        }

        if (frame.empty()) continue;

        // Виконуємо детекцію у фоні
        auto detections = detectSingleThread(frame);

        {
            std::lock_guard<std::mutex> lock(mtx);
            lastDetections = detections;
        }
    }
}
