#pragma once

#include <array>
#include <chrono>
#include <cstddef>

namespace cas::system {

enum class profile_id {
    update,
    render,
    traversal,
    coordinate_transform,
    batch_generator,
    geometry_render,
    run,

    count
};

class profiler {
public:
    static profiler& instance();

    bool begin(profile_id id);
    void end(profile_id id);

    void set_sample_limit(
        profile_id id,
        std::size_t limit
    );

    float get_ms(profile_id id) const;
    float get_average_ms(profile_id id) const;

    std::size_t get_sample_count(
        profile_id id
    ) const;

private:
    using clock = std::chrono::steady_clock;

    struct profile_data {
        clock::time_point start{};
        float last_ms{ 0.0f };
        float total_ms{ 0.0f };
        std::size_t sample_count{ 0 };
        std::size_t sample_limit{ 0 };
    };

    static constexpr std::size_t profile_count =
        static_cast<std::size_t>(profile_id::count);

    std::array<profile_data, profile_count> _profiles{};
};

}

