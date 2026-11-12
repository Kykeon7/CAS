#include "system/time.h"
#include <stdexcept>

namespace cas {

time& time::instance() {
	static time instance;
	return instance;
}

time::time() {
	_last_counter = SDL_GetPerformanceCounter();
	_frequency = SDL_GetPerformanceFrequency();

	if (_frequency == 0)
		throw std::runtime_error(
			"La frecuencia del contador de rendimiento de SDL no es válida."
		);
}

void time::tick() {
	const Uint64 current_counter = SDL_GetPerformanceCounter();

	_unscaled_delta_time =
		static_cast<double>(current_counter - _last_counter) /
		static_cast<double>(_frequency);

	_last_counter = current_counter;

	if (_is_paused)
		_delta_time = 0.0;
	else
		_delta_time = _unscaled_delta_time * _time_scale;
}

bool time::every(double interval, int id) {
	if (!std::isfinite(interval) || interval <= 0.0)
		throw std::invalid_argument(
			"El intervalo debe ser un valor finito positivo."
		);

	double& acc = _accumulators[id];

	acc += _delta_time;

	if (acc >= interval) {
		acc -= interval;
		return true;
	}

	return false;
}

void time::set_time_scale(double scale) {
	if (!std::isfinite(scale) || scale < 0.0) {
		throw std::invalid_argument(
			"La escala de tiempo debe ser un valor finito no negativo."
		);
	}

	_time_scale = scale;
}

} // namespace cas
