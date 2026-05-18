#pragma once

enum class Mode {
    NORMAL,
    INVERT,
    BLUR,
    CANNY,
    GLITCH
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
