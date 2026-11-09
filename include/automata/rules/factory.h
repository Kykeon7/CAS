#pragma once

#include "automata/rules/binary.h"
#include "automata/rules/spiral.h"

#include "automata/rules/beehive.h"

#include "automata/rules/rule_22_majority_memory.h"

#include <array>
#include <memory>
#include <string>

namespace cas::automata::rules {

std::unique_ptr<rule> string_to_rule(const std::string& str);
std::string rule_to_string(const rule& rule);

inline std::unique_ptr<rule> game_of_life() {
    return std::make_unique<binary>(
        std::array<bool, 9>{ false, false, false, true, false, false, false, false, false },
        std::array<bool, 9>{ false, false, true, true, false, false, false, false, false }
    );
}

inline std::unique_ptr<rule> high_life() {
    return std::make_unique<binary>(
        std::array<bool, 9>{ false, false, false, true, false, false, true, false, false },
        std::array<bool, 9>{ false, false, true, true, false, false, false, false, false }
    );
}

inline std::unique_ptr<rule> seeds() {
    return std::make_unique<binary>(
        std::array<bool, 9>{ false, false, true, false, false, false, false, false, false },
        std::array<bool, 9>{ false, false, false, false, false, false, false, false, false }
    );
}

inline std::unique_ptr<rule> spiral() {
    return std::make_unique<class spiral>();
}

inline std::unique_ptr<rule> beehive() {
    return std::make_unique<class beehive>();
}

inline std::unique_ptr<rule> rule_22_majority_memory() {
    return std::make_unique<
        class rule_22_majority_memory
    >();
}

} // namespace cas::automata::rules