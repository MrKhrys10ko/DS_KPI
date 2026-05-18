#include "KeyProcessor.hpp"

KeyProcessor::KeyProcessor() : currentMode(Mode::NORMAL), exitFlag(false) {}

void KeyProcessor::processKey(int key) {
    if (key == -1) return;
    switch (key) {
        case 27:
        case 'q': exitFlag = true; break;
        case '1': currentMode = Mode::NORMAL; break;
        case '2': currentMode = Mode::INVERT; break;
        case '3': currentMode = Mode::BLUR; break;
        case '4': currentMode = Mode::CANNY; break;
        case '5': currentMode = Mode::GLITCH; break;
        case '6': currentMode = Mode::FACE_SINGLE; break;
        case '7': currentMode = Mode::FACE_MULTI; break;
    }
}

Mode KeyProcessor::getMode() const { return currentMode; }
bool KeyProcessor::shouldExit() const { return exitFlag; }
