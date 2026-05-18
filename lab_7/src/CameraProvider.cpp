#include "CameraProvider.hpp"
#include <iostream>

CameraProvider::CameraProvider(int deviceId) {
    // 1. Пробуємо відкрити пристрій за замовчуванням (0)
    cap.open(deviceId);
    
    cv::Mat testFrame;
    if (cap.isOpened()) {
        cap >> testFrame; // Перевіряємо, чи йдуть кадри
    }
    
    // 2. Якщо індекс 0 не дав картини, пробуємо індекс 1 (/dev/video1)
    if (!cap.isOpened() || testFrame.empty()) {
        std::cout << "[INFO] Пристрій /dev/video0 не віддає відеопотік. Пробую /dev/video1..." << std::endl;
        cap.release();
        cap.open(1);
        if (cap.isOpened()) {
            cap >> testFrame;
        }
    }
    
    // 3. Якщо і з /dev/video1 глухо, перемикаємось на файл відео для підстраховки
    if (!cap.isOpened() || testFrame.empty()) {
        std::cout << "[INFO] Реальна камера не функціонує. Перемикаюсь на 'video.mp4'..." << std::endl;
        cap.release();
        cap.open("video.mp4");
        if (!cap.isOpened()) {
            std::cerr << "Помилка: Неможливо відкрити ні камеру, ні файл 'video.mp4'!" << std::endl;
        }
    }
}

CameraProvider::~CameraProvider() {
    if (cap.isOpened()) {
        cap.release();
    }
}

cv::Mat CameraProvider::getFrame() {
    cv::Mat frame;
    if (!cap.isOpened()) return frame;
    
    cap >> frame;
    
    // Якщо це відеофайл і він закінчився — пускаємо по колу
    if (frame.empty()) {
        // Перевіряємо чи це файл (в камери не можна скинути кадри в 0)
        cap.set(cv::CAP_PROP_POS_FRAMES, 0);
        cap >> frame;
    }
    
    return frame;
}

bool CameraProvider::isOpened() const {
    return cap.isOpened();
}
