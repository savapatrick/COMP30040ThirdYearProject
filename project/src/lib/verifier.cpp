//
// Created by Patrick Sava on 11/4/2020.
//

#include "verifier.h"
#include "operators.h"
#include "simplified_literal.h"
#include <stdexcept>

using namespace std;

namespace utils {
bool Verifier::isBalanced(const std::vector<std::string>& seq) {
    int open = 0;
    for(auto& elem : seq) {
        if(elem == "(") {
            open += 1;
        } else if(elem == ")") {
            open -= 1;
            if(open < 0) {
                return false;
            }
        }
    }
    return open == 0;
}

} // namespace utils
