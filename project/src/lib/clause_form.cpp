//
// Created by Patrick Sava on 11/18/2020.
//

#include "clause_form.h"
#include "operators.h"

using namespace std;

namespace utils {


std::string ClauseForm::getString(const Clause& clause) {
    Operators& operators = Operators::getInstance();
    string result;
    for(int ind = 0; ind < (int)clause.size(); ++ind) {
        auto literal = clause[ind];
        result += literal->getString();
        if(ind + 1 < (int)clause.size()) {
            result += operators.OR;
        }
    }
    return result;
}

std::string utils::ClauseForm::getString() const {
    Operators& operators = Operators::getInstance();
    string result;
    for(int ind = 0; ind < (int)literals.size(); ++ind) {
        result += getString(literals[ind]);
        if(ind + 1 < (int)literals.size()) {
            result += operators.AND;
        }
    }
    return result;
}

const vector<ClauseForm::Clause>& ClauseForm::getClauseForm() const {
    return literals;
}

}; // namespace utils
