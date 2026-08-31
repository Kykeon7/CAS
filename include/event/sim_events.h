#pragma once

namespace cas::event {

struct SimulationStarted{};
struct SimulationPaused{};
struct ToggleSimulation {};
struct Randomize {};
struct SimulationSpeedChanged {
	float speed;
};

}


