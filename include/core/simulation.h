#pragma once

#include <automaton/automaton.h>

namespace cas {

class Simulation {
public:
	Simulation();
	void step();
	void randomize();

	automaton& automaton() { return m_automaton; }

	void set_speed(float speed) { m_speed = speed; }
	float speed() const { return m_speed; }

	void toggle_cell(int x, int y);
	void toggle() { m_running = !m_running; }
	void resize(int width, int height);

	rule rule() const { return m_automaton.rule(); }

	void set_rule(const cas::rule& rule) {
		m_automaton = cas::automaton(
			m_automaton.width(),
			m_automaton.height(),
			m_automaton.topology(),
			rule
		);
	}

	int width() const { return m_automaton.width(); }
	int height() const { return m_automaton.height(); }

	bool is_running() const { return m_running; }
	int alive_count() const { return m_automaton.alive_count(); }

private:
	cas::automaton m_automaton;
	float m_speed = 10.0f;
	bool m_running = false;
};

} // namespace cas