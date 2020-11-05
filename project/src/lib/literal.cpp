//
// Created by Patrick Sava on 11/5/2020.
//

#include "literal.h"
#include <stdexcept>
#include "operators.h"

using namespace std;

namespace utils {

    bool Literal::isLiteral(const std::string &seq) {
        if (seq.empty()) {
            throw invalid_argument("cannot check whether the empty string is predicate");
        }
        Operators &operators = Operators::getInstance();
        int i = 0;
        if (operators.whichOperator(i, seq) == "NOT") {
            operators.advanceOperator(i, seq, "NOT");
        }
        return (i < (int) seq.size() and isupper(seq[i]));
    }

    std::string Literal::getString() const {
        string result;
        Operators& operators = Operators::getInstance();
        if (isNegated) {
            result += operators.NOT;
        }
        result += predicateName;
        result += "(";
        for (int ind = 0; ind < (int)arguments.size(); ++ ind) {
            if (ind + 1 == arguments.size()) {
                result += arguments[ind] + ")";
            }
            else {
                result += arguments[ind] + ",";
            }
        }
        return result;
    }

    const bool Literal::getIsNegated() const {
        return isNegated;
    }

    const string &Literal::getPredicateName() const {
        return predicateName;
    }

    const vector<std::string> &Literal::getArguments() const {
        return arguments;
    }
};