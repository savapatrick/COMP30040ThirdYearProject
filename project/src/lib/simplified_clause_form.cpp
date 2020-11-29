//
// Created by Patrick Sava on 11/18/2020.
//

#include "simplified_clause_form.h"
#include "operators.h"

using namespace std;

namespace utils {


std::string SimplifiedClauseForm::getString(const SimplifiedClause& clause) {
    Operators& operators = Operators::getInstance();
    string result;
    for(int ind = 0; ind < (int)clause.size(); ++ind) {
        auto simplifiedLiteral = clause[ind];
        result += simplifiedLiteral->getString();
        if(ind + 1 < (int)clause.size()) {
            result += operators.OR;
        }
    }
    return result;
}

std::string utils::SimplifiedClauseForm::getString() const {
    Operators& operators = Operators::getInstance();
    string result;
    for(int ind = 0; ind < (int)simplifiedLiterals.size(); ++ind) {
        result += getString(simplifiedLiterals[ind]);
        if(ind + 1 < (int)simplifiedLiterals.size()) {
            result += operators.AND;
        }
    }
    return result;
}

const vector<SimplifiedClauseForm::SimplifiedClause>& SimplifiedClauseForm::getSimplifiedClauseForm() const {
    return simplifiedLiterals;
}

}; // namespace utils
