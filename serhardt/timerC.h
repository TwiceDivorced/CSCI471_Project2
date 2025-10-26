//
// Created by Phillip Romig on 7/19/24.
//

#ifndef RFT_TIMERC_H
#define RFT_TIMERC_H
#include <chrono>


class timerC {
private:
    bool running;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::duration<float> duration;

public:
    timerC() : running(false), startTime(), duration(0) {};
   explicit timerC(int milliseconds) : running(false), startTime(),
        duration(std::chrono::milliseconds(milliseconds)) {};
    void setDuration(int milliseconds);
    void start();
    void stop();
    [[nodiscard]] bool timeout() const;
};


#endif //RFT_TIMERC_H
