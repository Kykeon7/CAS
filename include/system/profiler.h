#pragma once

#include <array>
#include <chrono>
#include <cstddef>

namespace cas::system {

/**
 * Identifies the operations tracked by the profiler.
 */
enum class profile_id {
    update,                 ///< Automaton update operations.
    render,                 ///< Rendering operations.
    traversal,              ///< Automaton traversal operations.
    coordinate_transform,   ///< Coordinate transformation operations.
    batch_generator,        ///< Batch generation operations.
    geometry_render,        ///< Geometry rendering operations.
    run,                    ///< Complete execution cycles.

    count                   ///< Number of available profiling identifiers.
};

/**
 * Collects timing statistics for predefined application operations.
 *
 * The profiler stores the most recent measurement, cumulative execution
 * time, sample count, and optional sample limit for each profile.
 *
 * A singleton instance provides access to the profiling data.
 */
class profiler {
public:
    /**
     * Returns the singleton profiler instance.
     *
     * @return Reference to the application-wide profiler.
     */
    static profiler& instance();

    /**
     * Starts timing an operation.
     *
     * If the profile has reached its configured sample limit,
     * no measurement is started.
     *
     * @param id Profile identifier.
     * @return True if timing was started, or false if the sample limit
     *         has been reached.
     */
    bool begin(profile_id id);

    /**
     * Stops timing an operation and records its duration.
     *
     * Updates the most recent measurement, cumulative execution time,
     * and sample count.
     *
     * @param id Profile identifier.
     */
    void end(profile_id id);

    /**
     * Sets the maximum number of measurements for a profile.
     *
     * A limit of zero disables the sample limit.
     *
     * @param id Profile identifier.
     * @param limit Maximum number of recorded measurements.
     */
    void set_sample_limit(
        profile_id id,
        std::size_t limit
    );

    /**
     * Returns the most recently recorded execution time.
     *
     * @param id Profile identifier.
     * @return Most recent duration in milliseconds, or zero if no
     *         measurement has been recorded.
     */
    [[nodiscard]]
    float get_ms(profile_id id) const;

    /**
     * Returns the average execution time across recorded measurements.
     *
     * @param id Profile identifier.
     * @return Average duration in milliseconds, or zero if no
     *         measurements have been recorded.
     */
    [[nodiscard]]
    float get_average_ms(profile_id id) const;

    /**
     * Returns the number of recorded measurements.
     *
     * @param id Profile identifier.
     * @return Number of completed measurements.
     */
    [[nodiscard]]
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

} // namespace cas::system
