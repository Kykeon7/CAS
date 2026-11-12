#include <system/profiler.h>

namespace cas::system {

profiler& profiler::instance() {
    static profiler instance;
    return instance;
}

bool profiler::begin(profile_id id) {
    auto& profile =
        _profiles[static_cast<std::size_t>(id)];

    if (
        profile.sample_limit != 0 &&
        profile.sample_count >= profile.sample_limit
        ) {
        return false;
    }

    profile.start = clock::now();

    return true;
}

void profiler::end(profile_id id) {
    auto& profile =
        _profiles[static_cast<std::size_t>(id)];

    const float ms =
        std::chrono::duration<float, std::milli>(
            clock::now() - profile.start
        ).count();

    profile.last_ms = ms;
    profile.total_ms += ms;
    ++profile.sample_count;
}

void profiler::set_sample_limit(
    profile_id id,
    std::size_t limit
) {
    _profiles[
        static_cast<std::size_t>(id)
    ].sample_limit = limit;
}

float profiler::get_ms(profile_id id) const {
    return _profiles[
        static_cast<std::size_t>(id)
    ].last_ms;
}

float profiler::get_average_ms(profile_id id) const {
    const auto& profile =
        _profiles[static_cast<std::size_t>(id)];

    if (profile.sample_count == 0)
        return 0.0f;

    return profile.total_ms /
        static_cast<float>(profile.sample_count);
}

std::size_t profiler::get_sample_count(
    profile_id id
) const {
    return _profiles[
        static_cast<std::size_t>(id)
    ].sample_count;
}

} // namespace cas::system
