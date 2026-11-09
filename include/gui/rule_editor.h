#pragma once

#include <automata/rules/factory.h>
#include <simulations/simulation.h>

#include <memory>
#include <string>

namespace cas::gui {

class rule_editor {
public:
    void open(cas::simulations::simulation& sim);
    void render(cas::simulations::simulation& sim);

    std::string rule_display_name(const automata::rules::rule& current) const;

private:
    void render_popup(cas::simulations::simulation& sim);
    void render_presets();
    void render_string();
    void render_checkboxes();
    void render_actions(cas::simulations::simulation& sim);

    void sync_rule_buffer();
    void set_temp_rule(std::unique_ptr<automata::rules::rule> new_rule);

    std::unique_ptr<automata::rules::rule> _temp_rule;
    char _rule_buffer[32] = "B3/S23";
};

}