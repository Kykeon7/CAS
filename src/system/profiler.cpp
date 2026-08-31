#include <system/profiler.h>

namespace cas::system {

    Profiler& Profiler::instance() {
        static Profiler p;
        return p;
    }

    void Profiler::begin(const std::string& name) {
        m_start[name] = Clock::now();
    }

    void Profiler::end(const std::string& name) {
        auto now = Clock::now();
        auto it = m_start.find(name);

        if (it == m_start.end()) return;

        float ms = std::chrono::duration<float, std::milli>(now - it->second).count();
        m_times[name] = ms;
    }

    float Profiler::get_ms(const std::string& name) const {
        auto it = m_times.find(name);
        if (it == m_times.end()) return 0.0f;
        return it->second;
    }

} // namespace cas::system