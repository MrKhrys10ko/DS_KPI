#include "FrameProcessor.hpp"
#include "FaceDetector.hpp"
#include <vector>

FrameProcessor::FrameProcessor() : brightness(50), isDrawing(false) {}

cv::Mat FrameProcessor::process(const cv::Mat& input, Mode mode, FaceDetector* faceDetector) {
    if (input.empty()) return input;

    if (canvas.empty() || canvas.size() != input.size()) {
        canvas = cv::Mat::zeros(input.size(), input.type());
    }

    cv::Mat output = input.clone();

    switch (mode) {
        case Mode::INVERT:
            cv::bitwise_not(output, output);
            break;
        case Mode::BLUR:
            cv::GaussianBlur(output, output, cv::Size(15, 15), 0);
            break;
        case Mode::CANNY:
            cv::cvtColor(output, output, cv::COLOR_BGR2GRAY);
            cv::Canny(output, output, 50, 150);
            cv::cvtColor(output, output, cv::COLOR_GRAY2BGR);
            break;
        case Mode::GLITCH: {
            std::vector<cv::Mat> channels;
            cv::split(output, channels);
            cv::Mat shiftMat = cv::Mat::zeros(output.size(), CV_8UC1);
            int shiftOffset = 15;
            cv::Mat roi = channels[2](cv::Rect(0, 0, output.cols - shiftOffset, output.rows));
            cv::Mat target = shiftMat(cv::Rect(shiftOffset, 0, output.cols - shiftOffset, output.rows));
            roi.copyTo(target);
            channels[2] = shiftMat;
            cv::merge(channels, output);
            break;
        }
        case Mode::FACE_SINGLE: {
            if (faceDetector) {
                auto detections = faceDetector->detectSingleThread(output);
                for (const auto& rect : detections) {
                    cv::rectangle(output, rect, cv::Scalar(0, 0, 255), 3);
                }
            }
            break;
        }
        case Mode::FACE_MULTI: {
            if (faceDetector) {
                faceDetector->updateFrame(output);
                auto detections = faceDetector->getDetections();
                for (const auto& rect : detections) {
                    cv::rectangle(output, rect, cv::Scalar(0, 255, 0), 3);
                }
            }
            break;
        }
        case Mode::NORMAL:
        default:
            break;
    }

    output.convertTo(output, -1, 1, (brightness - 50) * 2);

    cv::Mat mask;
    cv::cvtColor(canvas, mask, cv::COLOR_BGR2GRAY);
    cv::threshold(mask, mask, 1, 255, cv::THRESH_BINARY);
    canvas.copyTo(output, mask);

    std::string modeText = "Mode: ";
    if (mode == Mode::NORMAL) modeText += "1: NORMAL";
    else if (mode == Mode::INVERT) modeText += "2: INVERT";
    else if (mode == Mode::BLUR) modeText += "3: BLUR";
    else if (mode == Mode::CANNY) modeText += "4: CANNY";
    else if (mode == Mode::GLITCH) modeText += "5: GLITCH";
    else if (mode == Mode::FACE_SINGLE) modeText += "6: FACE SINGLE (LAG)";
    else if (mode == Mode::FACE_MULTI) modeText += "7: FACE MULTI (SMOOTH)";

    cv::putText(output, modeText, cv::Point(10, 30), cv::FONT_HERSHEY_SIMPLEX, 0.8, cv::Scalar(0, 255, 0), 2);
    cv::putText(output, "ESC/Q to Exit | R-Click to clear draw", cv::Point(10, 60), cv::FONT_HERSHEY_SIMPLEX, 0.5, cv::Scalar(255, 255, 255), 1);

    return output;
}

void FrameProcessor::mouseCallback(int event, int x, int y, int flags, void* userdata) {
    FrameProcessor* fp = reinterpret_cast<FrameProcessor*>(userdata);
    if (event == cv::EVENT_LBUTTONDOWN) {
        fp->isDrawing = true;
        fp->mousePos = cv::Point(x, y);
    } else if (event == cv::EVENT_MOUSEMOVE && fp->isDrawing) {
        cv::Point newPos(x, y);
        cv::line(fp->canvas, fp->mousePos, newPos, cv::Scalar(0, 0, 255), 3);
        fp->mousePos = newPos;
    } else if (event == cv::EVENT_LBUTTONUP) {
        fp->isDrawing = false;
    } else if (event == cv::EVENT_RBUTTONDOWN) {
        fp->canvas.setTo(cv::Scalar(0,0,0));
    }
}
