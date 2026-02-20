#pragma once

namespace cas {

enum class EventType {
	TogglePause,
	ResetSimulation,
	StepSimulation,
	Quit
};

struct Event { EventType type; };

class EventBus {
public:
	static EventBus& instance();
	void emit(const )
};

} // namespace cas