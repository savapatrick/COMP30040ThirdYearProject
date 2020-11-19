//
// Created by Patrick Sava on 11/4/2020.
//

#include "verifier.h"
#include "literal.h"
#include "operators.h"
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

bool Verifier::areAllDNFs(const vector<std::vector<std::string>>& terms) {
    static Operators& operators = Operators::getInstance();
    for(auto& term : terms) {
        if(term.size() % 2 == 1) {
            for(int ind = 0; ind < (int)term.size(); ++ind) {
                if(ind % 2 == 1) {
                    if(operators.whichOperator(ind, term[ind]) != "OR") {
                        return false;
                    }
                } else {
                    if(!Literal::isLiteral(term[ind])) {
                        return false;
                    }
                }
            }
        } else {
            return false;
        }
    }
    return true;
}
} // namespace utils
