#pragma once

#include <array>
#include <memory>
#include <string>

#include <automata/rules/beehive.h>
#include <automata/rules/binary.h>
#include <automata/rules/rule.h>
#include <automata/rules/rule_22_majority_memory.h>
#include <automata/rules/spiral.h>

namespace cas::automata::rules {

/// @brief Creates a rule from its string representation.
/// @param str String representation of the rule.
/// @return Pointer to the created rule, or nullptr if the string is invalid
///         or represents an unsupported rule.
std::unique_ptr<rule> string_to_rule(const std::string& str);

/// @brief Converts a rule into its string representation.
/// @param rule Rule to serialize.
/// @return String representation of the rule, or "Unknown" if the rule is unsupported.
std::string rule_to_string(const rule& rule);

/// @brief Creates Conway's Game of Life rule.
/// @return Pointer to the created rule.
inline std::unique_ptr<rule> game_of_life() {
    return std::make_unique<binary>(
        std::array<bool, 9>{
            false, false, false, true, false, false, false, false, false
        },
        std::array<bool, 9>{
            false, false, true, true, false, false, false, false, false
        }
    );
}

/// @brief Creates the HighLife rule.
/// @return Pointer to the created rule.
inline std::unique_ptr<rule> high_life() {
    return std::make_unique<binary>(
        std::array<bool, 9>{
            false, false, false, true, false, false, true, false, false
        },
        std::array<bool, 9>{
            false, false, true, true, false, false, false, false, false
        }
    );
}

/// @brief Creates the Seeds rule.
/// @return Pointer to the created rule.
inline std::unique_ptr<rule> seeds() {
    return std::make_unique<binary>(
        std::array<bool, 9>{
            false, false, true, false, false, false, false, false, false
        },
        std::array<bool, 9>{
            false, false, false, false, false, false, false, false, false
        }
    );
}

/// @brief Creates the Spiral rule.
/// @return Pointer to the created rule.
inline std::unique_ptr<rule> spiral() {
    return std::make_unique<class spiral>();
}

/// @brief Creates the Beehive rule.
/// @return Pointer to the created rule.
inline std::unique_ptr<rule> beehive() {
    return std::make_unique<class beehive>();
}

/// @brief Creates the Rule 22 Majority Memory rule.
/// @return Pointer to the created rule.
inline std::unique_ptr<rule> rule_22_majority_memory() {
    return std::make_unique<class rule_22_majority_memory>();
}

} // namespace cas::automata::rules