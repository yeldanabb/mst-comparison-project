#ifndef TIMER_HPP
#define TIMER_HPP
#include <chrono>
#include <string>

class Timer {
private:
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool running = false;

public:
    void start() {
        startTime = std::chrono::high_resolution_clock::now();
        running = true;
    }
    
    void stop() {
        endTime = std::chrono::high_resolution_clock::now();
        running = false;
    }
    
    double elapsedMilliseconds() const {
        auto end = running ? std::chrono::high_resolution_clock::now() : endTime;
        return std::chrono::duration<double, std::milli>(end - startTime).count();
    }
    
    double elapsedSeconds() const {
        return elapsedMilliseconds() / 1000.0;
    }
};

#endif