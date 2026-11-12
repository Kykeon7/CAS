
#pragma once

#include <automata/rules/factory.h>
#include <simulations/simulation.h>

#include <memory>
#include <string>

namespace cas::gui {

/**
 * @brief Provides an interface for editing cellular automaton rules.
 *
 * Changes are kept in a temporary rule until the user applies them.
 */
class rule_editor final {
public:
    rule_editor() = default;

    rule_editor(const rule_editor&) = delete;
    rule_editor& operator=(const rule_editor&) = delete;

    rule_editor(rule_editor&&) noexcept = default;
    rule_editor& operator=(rule_editor&&) noexcept = default;

    ~rule_editor() = default;

    /**
     * @brief Opens the rule editor using the simulation's current rule.
     *
     * @param sim Simulation whose rule will be edited.
     */
    void open(cas::simulations::simulation& sim);

    /**
     * @brief Renders the rule editor popup.
     *
     * @param sim Simulation whose rule may be updated.
     */
    void render(cas::simulations::simulation& sim);

    /**
     * @brief Returns a display name for the specified rule.
     *
     * @param current Rule to identify.
     * @return Human-readable rule name.
     */
    [[nodiscard]]
    std::string rule_display_name(
        const automata::rules::rule& current
    ) const;

private:
    void render_popup(cas::simulations::simulation& sim);
    void render_presets();
    void render_string();
    void render_checkboxes();
    void render_actions(cas::simulations::simulation& sim);

    void sync_rule_buffer();

    void set_temp_rule(
        std::unique_ptr<automata::rules::rule> new_rule
    );

    std::unique_ptr<automata::rules::rule> _temp_rule;
    char _rule_buffer[32] = "B3/S23";
};

} // namespace cas::gui
