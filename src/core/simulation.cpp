#include <core/simulation.h>
#include <automaton/topologies.h>
#include <automaton/rules.h>

namespace cas {

Simulation::Simulation()
		: m_automaton(100
		, 100
		, topologies::HexFinite()
		, rules::GameOfLife())
{ }

void Simulation::step() {
	m_automaton.step();
}

void Simulation::randomize() {
	m_automaton.randomize();
}

void Simulation::toggle_cell(int x, int y) {
	if (!m_automaton.in_bounds(x, y)) return;

	m_automaton.toggle(x, y);
}

void Simulation::resize(int width, int height) {
	if (width < 1 || height < 1) return;

	m_automaton.resize(width, height);
}

} // namespace cas