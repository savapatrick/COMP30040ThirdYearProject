//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_VERIFIER_H
#define PROJECT_VERIFIER_H

#include <string>
#include <vector>

namespace utils {
class Verifier {
    public:
    Verifier() {
    }

    // function which matches if the sequence is balanced
    // parantheses-wise
    static bool isBalanced(const std::vector<std::string>& seq);

    static bool areAllDNFs(const std::vector<std::vector<std::string>>& terms);
};
}; // namespace utils

#endif // PROJECT_VERIFIER_H
