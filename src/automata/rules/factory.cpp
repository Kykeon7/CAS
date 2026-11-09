#include <automata/rules/factory.h>

#include <cctype>
#include <memory>

namespace cas::automata::rules {


std::unique_ptr<rule> string_to_rule(const std::string& str) {
    if (str == "R22maj:4") {
        return rule_22_majority_memory();
    }

    std::array<bool, 9> birth{};
    std::array<bool, 9> survive{};

    bool parsing_birth = false;
    bool parsing_survive = false;

    for (char c : str) {
        if (c == 'B') {
            parsing_birth = true;
            parsing_survive = false;
        }
        else if (c == 'S') {
            parsing_birth = false;
            parsing_survive = true;
        }
        else if (std::isdigit(static_cast<unsigned char>(c))) {
            int n = c - '0';

            if (parsing_birth)
                birth[n] = true;

            if (parsing_survive)
                survive[n] = true;
        }
    }

    return std::make_unique<binary>(
        birth,
        survive
    );
}


std::string rule_to_string(const rule& rule) {
    if (const auto* binary = dynamic_cast<const class binary*>(&rule)) {
        std::string result = "B";

        for (int i = 0; i < 9; ++i) {
            if (binary->birth()[i])
                result += std::to_string(i);
        }

        result += "/S";

        for (int i = 0; i < 9; ++i) {
            if (binary->survive()[i])
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