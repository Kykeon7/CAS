#include <automata/rules/factory.h>

#include <cctype>
#include <memory>
#include <string>

namespace cas::automata::rules {

    std::unique_ptr<rule> string_to_rule(const std::string& str) {
        if (str == "Spiral")
            return spiral();

        if (str == "Beehive")
            return beehive();

        if (str == "R22maj:4")
            return rule_22_majority_memory();

        if (str.size() < 3 || str[0] != 'B')
            return nullptr;

        const std::size_t separator = str.find('/');

        if (separator == std::string::npos ||
            separator + 1 >= str.size() ||
            str[separator + 1] != 'S')
        {
            return nullptr;
        }

        if (str.find('/', separator + 1) != std::string::npos)
            return nullptr;

        std::array<bool, 9> birth{};
        std::array<bool, 9> survive{};

        const auto parse_states = [](
            const std::string& value,
            std::size_t begin,
            std::size_t end,
            std::array<bool, 9>& states
            ) -> bool {
                for (std::size_t i = begin; i < end; ++i) {
                    const char c = value[i];

                    if (c < '0' || c > '8')
                        return false;

                    const std::size_t state = c - '0';

                    if (states[state])
                        return false;

                    states[state] = true;
                }

                return true;
            };

        if (!parse_states(str, 1, separator, birth))
            return nullptr;

        if (!parse_states(str, separator + 2, str.size(), survive))
            return nullptr;

        return std::make_unique<binary>(
            birth,
            survive
        );
    }

std::string rule_to_string(const rule& rule) {
    if (const auto* binary_rule = dynamic_cast<const binary*>(&rule)) {
        std::string result = "B";

        for (int i = 0; i < 9; ++i) {
            if (binary_rule->birth()[i])
                result += std::to_string(i);
        }

        result += "/S";

        for (int i = 0; i < 9; ++i) {
            if (binary_rule->survive()[i])
                result += std::to_string(i);
        }

        return result;
    }

    if (dynamic_cast<const class spiral*>(&rule))
        return "Spiral";

    if (dynamic_cast<const class beehive*>(&rule))
        return "Beehive";

    if (dynamic_cast<const class rule_22_majority_memory*>(&rule))
        return "R22maj:4";

    return "Unknown";
}

} // namespace cas::automata::rules