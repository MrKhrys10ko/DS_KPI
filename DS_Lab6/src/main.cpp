#include "CameraProvider.hpp"
#include "KeyProcessor.hpp"
#include "FrameProcessor.hpp"
#include "Display.hpp"

int main() {
    CameraProvider camera(0);
    if (!camera.isOpened()) return -1;

    KeyProcessor keyProc;
    FrameProcessor frameProc;
    Display display("Lab 6 - OpenCV Viewer");

    cv::createTrackbar("Brightness", display.getWindowName(), &frameProc.brightness, 100);
    cv::setMouseCallback(display.getWindowName(), FrameProcessor::mouseCallback, &frameProc);

    while (!keyProc.shouldExit()) {
        cv::Mat frame = camera.getFrame();
        if (frame.empty()) break;

        cv::Mat processed = frameProc.process(frame, keyProc.getMode());
        display.show(processed);

        int key = cv::waitKey(30);
        keyProc.processKey(key);
    }
    return 0;
}
