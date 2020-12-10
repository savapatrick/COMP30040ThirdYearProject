//
// Created by Patrick Sava on 11/5/2020.
//

#include "simplified_literal.h"
#include "operators.h"
#include <stdexcept>

using namespace std;

namespace utils {

bool SimplifiedLiteral::isSimplifiedLiteral(const std::string& seq) {
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

std::string SimplifiedLiteral::getArgumentString(const arg& argument) {
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

std::string SimplifiedLiteral::getString() const {
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

bool SimplifiedLiteral::getIsNegated() const {
    return isNegated;
}

const string& SimplifiedLiteral::getPredicateName() const {
    return predicateName;
}

const std::vector<SimplifiedLiteral::arg>& SimplifiedLiteral::getArguments() const {
    return arguments;
}

void SimplifiedLiteral::negate() {
    isNegated ^= true;
}

void SimplifiedLiteral::setArguments(const vector<SimplifiedLiteral::arg>& args) {
    SimplifiedLiteral::arguments = args;
}

bool SimplifiedLiteral::substituteSkolem(std::unordered_map<std::string, SimplifiedLiteral::arg>& skolem) {
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
void SimplifiedLiteral::simpleSubstitution(unordered_map<std::string, std::string>& substitution) {
    for(auto& argument : arguments) {
        if(argument.index() == 0) {
            // this is variable
            // only raw variables could be substituted
            auto variable = get<0>(argument);
            if(substitution.find(variable) != substitution.end()) {
                argument = substitution[variable];
            }
        } else {
            auto& variables = get<1>(argument).second;
            for(auto& variable : variables) {
                if(substitution.find(variable) != substitution.end()) {
                    variable = substitution[variable];
                }
            }
        }
    }
}
std::unordered_set<std::string> SimplifiedLiteral::getAllVariablesAndConstants() const {
    unordered_set<string> variablesAndConstants;
    for(auto& argument : arguments) {
        if(argument.index() == 0) {
            variablesAndConstants.insert(get<0>(argument));
        } else {
            auto args = get<1>(argument).second;
            for(auto& arg : args) { variablesAndConstants.insert(arg); }
        }
    }
    return variablesAndConstants;
}
}; // namespace utils