#include <chrono>

class Timer {
private:
    std::chrono::time_point<std::chrono::steady_clock> start_time;
    bool is_running;

public:
    Timer() : is_running(false) {}

    void start() {
        if (!is_running) {
            start_time = std::chrono::steady_clock::now();
            is_running = true;
        }
    }

    int stop() {
        if (is_running) {
            auto end_time = std::chrono::steady_clock::now();
            is_running = false;
            return std::chrono::duration_cast<std::chrono::minutes>(end_time - start_time).count();
        }
        return 0;
    }
};