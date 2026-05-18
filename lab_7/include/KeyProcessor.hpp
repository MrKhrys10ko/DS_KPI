#pragma once

enum class Mode {
    NORMAL,
    INVERT,
    BLUR,
    CANNY,
    GLITCH,
    FACE_SINGLE,
    FACE_MULTI
};

class KeyProcessor {
private:
    Mode currentMode;
    bool exitFlag;
public:
    KeyProcessor();
    void processKey(int key);
    Mode getMode() const;
    bool shouldExit() const;
};
