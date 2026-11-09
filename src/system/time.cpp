#include "system/time.h"

namespace cas {

time& time::instance() {
	static time instance;
	return instance;
}

time::time() {
	m_last_counter = SDL_GetPerformanceCounter();
}

void time::tick() {
	const Uint64 current_counter = SDL_GetPerformanceCounter();
	const Uint64 frequency = SDL_GetPerformanceFrequency();

	m_unscaled_delta_time =
		static_cast<double>(current_counter - m_last_counter) /
		static_cast<double>(frequency);

	m_last_counter = current_counter;

	if (m_is_paused)
		m_delta_time = 0.0;
	else
		m_delta_time = m_unscaled_delta_time * m_time_scale;
}

bool time::every(double interval, int id) {
	if (interval <= 0.0)
		return false;

	double& acc = m_accumulators[id];

	acc += m_delta_time;

	if (acc >= interval) {
		acc -= interval;
		return true;
	}

	return false;
}

double time::delta_time() const { return m_delta_time; }
double time::unscaled_delta_time() const { return m_unscaled_delta_time; }
void time::pause() { m_is_paused = true; }
void time::resume() { m_is_paused = false; }
bool time::is_paused() const { return m_is_paused; }
void time::set_time_scale(double scale) { m_time_scale = scale; }
double time::time_scale() const { return m_time_scale; }
} // namespace cas
