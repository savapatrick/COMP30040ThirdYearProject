//
// Created by Patrick Sava on 11/4/2020.
//

#include "operators.h"
#include <algorithm>

using namespace std;

namespace utils {
string Operators::whichOperator(const int& position, const std::string& seq) {
    if(position >= (int)seq.size()) {
        throw out_of_range("position beyond end of seq");
    }
    string aux;
    aux += seq[position];
    if(inv.find(aux) != inv.end()) {
        return inv[aux];
    }
    if(position + 1 < (int)seq.size()) {
        aux += seq[position + 1];
        if(inv.find(aux) != inv.end()) {
            return inv[aux];
        }
        if(position + 2 < (int)seq.size()) {
            aux += seq[position + 2];
            if(inv.find(aux) != inv.end()) {
                return inv[aux];
            }
        }
    }
    return "none";
}

string Operators::advanceOperator(int& position, const string& seq, const string& result) {
    string answer;
    if(mapping.find(result) != mapping.end()) {
        for(int ind = position; ind < position + mapping[result].size(); ++ind) { answer += seq[ind]; }
        position += mapping[result].size();
    } else {
        if(result == "none") {
            throw logic_error("advanceOperator should always return an operator");
        }
        throw out_of_range("result of the operator was altered");
    }
    return answer;
}

bool Operators::isQuantifier(const string& current) {
    return current == EQuantifier or current == VQuantifier;
}

bool Operators::isQuantifierAndVariable(const string& current) {
    if(!isQuantifier(current.substr(0, 1))) {
        return false;
    }
    return (
    std::find_if_not(current.begin() + 1, current.end(), [](char c) { return islower(c) or c == '_'; }) == current.end());
}

std::string Operators::getQuantifierFromQuantifierAndVariable(const string& current) {
    if(!isQuantifierAndVariable(current)) {
        throw invalid_argument("expected quantifier and variable but given " + current);
    }
    return current.substr(0, 1);
}

std::string Operators::getVariableFromQuantifierAndVariable(const string& current) {
    if(!isQuantifierAndVariable(current)) {
        throw invalid_argument("expected quantifier and variable but given " + current);
    }
    return current.substr(1);
}

std::string Operators::getOperator(const string& which) {
    if(mapping.find(which) == mapping.end()) {
        throw invalid_argument("the given argument is not an operator; argument: " + which);
    }
    return mapping[which];
}

std::string Operators::flipQuantifier(const string& which) {
    if(!isQuantifier(which)) {
        throw invalid_argument("expected quantifier but given " + which);
    }
    if(which == EQuantifier)
        return VQuantifier;
    return EQuantifier;
}

bool Operators::isNot(const string& which) {
    return (which == NOT);
}

bool Operators::isAnd(const string& which) {
    return (which == AND);
}

bool Operators::isOr(const string& which) {
    return (which == OR);
}

std::string Operators::flipAndOrOr(const string& which) {
    if(!isAnd(which) and !isOr(which)) {
        throw invalid_argument("the given argument is not AND or OR; given " + which);
    }
    if(isAnd(which)) {
        return OR;
    }
    return AND;
}
} // namespace utils
