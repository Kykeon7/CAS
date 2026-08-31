#pragma once

#include <automaton/rule.h>
#include <string>

namespace cas::rules {

rule string_to_rule(const std::string& str);
std::string rule_to_string(const rule& rule);

inline rule GameOfLife() {
    rule r{};

    r.birth[3] = true;

    r.survive[2] = true;
    r.survive[3] = true;

    return r;
}

inline rule HighLife() {
    rule r{};

    r.birth[3] = true;
    r.birth[6] = true;

    r.survive[2] = true;
    r.survive[3] = true;

    return r;
}

inline rule Seeds() {
    rule r{};

    r.birth[2] = true;

    return r;
}

} // namespace cas::rules