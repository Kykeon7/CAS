#include <automaton/rules.h>

namespace cas::rules {

rule string_to_rule(const std::string& str) {
    rule rule{};

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
        else if (std::isdigit(c)) {
            int n = c - '0';

            if (n >= 0 && n <= 8) {
                if (parsing_birth)
                    rule.birth[n] = true;
                if (parsing_survive)
                    rule.survive[n] = true;
            }
        }
    }

    return rule;
}

std::string rule_to_string(const rule& rule) {
    std::string result = "B";

    for (int i = 0; i < 9; i++)
        if (rule.birth[i])
            result += std::to_string(i);

    result += "/S";

    for (int i = 0; i < 9; i++)
        if (rule.survive[i])
            result += std::to_string(i);

    return result;
}

} // namespace cas::rules