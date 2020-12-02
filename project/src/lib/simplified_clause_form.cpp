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
    for(int ind = 0; ind < (int)simplifiedClauseForm.size(); ++ind) {
        result += getString(simplifiedClauseForm[ind]);
        if(ind + 1 < (int)simplifiedClauseForm.size()) {
            result += operators.AND;
        }
    }
    return result;
}

const vector<SimplifiedClauseForm::SimplifiedClause>& SimplifiedClauseForm::getSimplifiedClauseForm() const {
    return simplifiedClauseForm;
}
std::unordered_set<std::string> SimplifiedClauseForm::getAllArguments() const {
    return allArguments;
}

}; // namespace utils
