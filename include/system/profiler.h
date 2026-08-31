#pragma once
#include <unordered_map>
#include <string>
#include <chrono>

namespace cas::system {

class Profiler {
public:
    static Profiler& instance();

    void begin(const std::string& name);
    void end(const std::string& name);

    float get_ms(const std::string& name) const;

private:
    using Clock = std::chrono::high_resolution_clock;

    std::unordered_map<std::string, Clock::time_point> m_start;
    std::unordered_map<std::string, float> m_times;
};

} // namespace cas::system