//
// Created by Patrick Sava on 11/5/2020.
//

#include "literal.h"
#include "operators.h"
#include <stdexcept>

using namespace std;

namespace utils {

bool Literal::isLiteral(const std::string& seq) {
    if(seq.empty()) {
        throw invalid_argument("cannot check whether the empty string is predicate");
    }
    Operators& operators = Operators::getInstance();
    int i                = 0;
    if(operators.whichOperator(i, seq) == "NOT") {
        operators.advanceOperator(i, seq, "NOT");
    }
    return (i < (int)seq.size() and isupper(seq[i]));
}

std::string Literal::getArgumentString(const arg& argument) {
    if(argument.index() == 0) {
        return get<0>(argument);
    }
    string result;
    auto function = get<1>(argument);
    result += function.first;
    result += "(";
    for(auto& functionArgument : function.second) { result += functionArgument + ","; }
    result.pop_back();
    result += ")";
    return result;
}

std::string Literal::getString() const {
    string result;
    Operators& operators = Operators::getInstance();
    if(isNegated) {
        result += operators.NOT;
    }
    result += predicateName;
    result += "(";
    for(int ind = 0; ind < (int)arguments.size(); ++ind) {
        if(ind + 1 == arguments.size()) {
            result += getArgumentString(arguments[ind]) + ")";
        } else {
            result += getArgumentString(arguments[ind]) + ",";
        }
    }
    return result;
}

bool Literal::getIsNegated() const {
    return isNegated;
}

const string& Literal::getPredicateName() const {
    return predicateName;
}

const std::vector<Literal::arg>& Literal::getArguments() const {
    return arguments;
}

void Literal::negate() {
    isNegated ^= true;
}

void Literal::setArguments(const vector<Literal::arg>& args) {
    Literal::arguments = args;
}

bool Literal::substituteSkolem(std::unordered_map<std::string, Literal::arg>& skolem) {
    bool wasModified = false;
    for(auto& argument : arguments) {
        if(argument.index() == 0) {
            // this is variable
            // only raw variables could be substituted
            auto variable = get<0>(argument);
            if(skolem.find(variable) != skolem.end()) {
                argument    = skolem[variable];
                wasModified = true;
            }
        }
    }
    return wasModified;
}
void Literal::simpleSubstitution(unordered_map<std::string, std::string>& substitution) {
    for(auto& argument : arguments) {
        if(argument.index() == 0) {
            // this is variable
            // only raw variables could be substituted
            auto variable = get<0>(argument);
            if(substitution.find(variable) != substitution.end()) {
                argument = substitution[variable];
            }
        }
    }
}
}; // namespace utils