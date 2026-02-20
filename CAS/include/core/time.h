#pragma once

#include <SDL3/SDL.h>
#include <unordered_map>

namespace cas {

class Time {
public:
	static Time& instance();

	void tick();

	double delta_time() const;
	double unscaled_delta_time() const;

	void pause();
	void resume();
	bool is_paused() const;

	void set_time_scale(double scale);
	double time_scale() const;

	bool every(double interval, int id = 0);

private:
	Time();
	~Time() = default;

	Time(const Time&) = delete;
	Time& operator=(const Time&) = delete;

private:
	Uint64 m_last_counter = 0;
	double m_delta_time = 0.0;
	double m_unscaled_delta_time = 0.0;

	double m_time_scale = 1.0;
	bool m_is_paused = false;

	std::unordered_map<int, double> m_accumulators;
};

} // namespace cas