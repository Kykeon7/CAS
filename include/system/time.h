#pragma once

#include <SDL3/SDL.h>

#include <unordered_map>

namespace cas {

/**
 * Manages application timing and time scaling.
 *
 * Maintains frame timing information using SDL's high-resolution
 * performance counter. Supports pausing, time scaling, and
 * independent interval accumulators.
 *
 * A singleton instance provides global access to the time manager.
 */
class time {
public:
    /**
     * Returns the singleton time manager instance.
     *
     * @return Reference to the application-wide time manager.
     */
    static time& instance();

    /**
     * Updates frame timing information.
     *
     * Calculates the elapsed real time since the previous call and
     * updates the scaled delta time according to the current pause
     * state and time scale.
     */
    void tick();

    /**
     * Returns the scaled time elapsed since the previous tick.
     *
     * Returns zero while the time manager is paused.
     *
     * @return Scaled frame duration in seconds.
     */
    [[nodiscard]]
    double delta_time() const {
        return _delta_time;
    }

    /**
     * Returns the real time elapsed since the previous tick.
     *
     * This value is unaffected by pausing or time scaling.
     *
     * @return Unscaled frame duration in seconds.
     */
    [[nodiscard]]
    double unscaled_delta_time() const {
        return _unscaled_delta_time;
    }

    /** Pauses the scaled application time. */
    void pause() {
        _is_paused = true;
    }

    /** Resumes the scaled application time. */
    void resume() {
        _is_paused = false;
    }

    /**
     * Checks whether the time manager is paused.
     *
     * @return True if the time manager is paused.
     */
    [[nodiscard]]
    bool is_paused() const {
        return _is_paused;
    }

    /**
     * Sets the time scaling factor.
     *
     * A scale of 1.0 represents real-time progression, values greater
     * than 1.0 accelerate time, and values between 0.0 and 1.0 slow
     * it down.
     *
     * @param scale Time scaling factor.
     */
    void set_time_scale(double scale);

    /**
     * Returns the current time scaling factor.
     *
     * @return Current time scaling factor.
     */
    [[nodiscard]]
    double time_scale() const {
        return _time_scale;
    }

    /**
     * Checks whether a specified interval has elapsed.
     *
     * Maintains an independent accumulator for each identifier.
     * The accumulator advances using scaled delta time. When the
     * interval is reached, the accumulated interval is subtracted
     * and the function returns true.
     *
     * @param interval Interval duration in seconds.
     * @param id Identifier of the independent interval accumulator.
     * @return True if the interval has elapsed, or false otherwise.
     * @throws std::invalid_argument If the interval is not finite and positive.
     */
    bool every(double interval, int id = 0);

private:
    time();
    ~time() = default;

    time(const time&) = delete;
    time& operator=(const time&) = delete;

    time(time&&) = delete;
    time& operator=(time&&) = delete;

private:
    Uint64 _frequency = 0;
    Uint64 _last_counter = 0;
    double _delta_time = 0.0;
    double _unscaled_delta_time = 0.0;

    double _time_scale = 1.0;
    bool _is_paused = false;

    std::unordered_map<int, double> _accumulators;
};

} // namespace cas
