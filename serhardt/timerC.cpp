//
// Created by Phillip Romig on 7/19/24.
//

#include "timerC.h"
#include <stdexcept>

void timerC::setDuration(int milliseconds) {
    if (running)
        throw std::runtime_error("Cannot set duration while timer is running");
    duration = std::chrono::milliseconds(milliseconds);
}

void timerC::start() {
    startTime = std::chrono::high_resolution_clock::now();
    running = true;
}

void timerC::stop() {
    running = false;
}


bool timerC::timeout() const {
    if (running) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - startTime);
        return elapsedTime >= duration;
    }
    return false;
}
