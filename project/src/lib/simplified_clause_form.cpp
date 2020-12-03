//
// Created by Patrick Sava on 11/18/2020.
//

#include "simplified_clause_form.h"
#include "operators.h"
#include <algorithm>

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
std::shared_ptr<SimplifiedClauseForm> SimplifiedClauseForm::clone() const {
    std::vector<SimplifiedClauseForm::SimplifiedClause> simplifiedClauseFormCopy;
    simplifiedClauseFormCopy.reserve(simplifiedClauseForm.size());
    for (auto &simplifiedClause : simplifiedClauseForm) {
        SimplifiedClauseForm::SimplifiedClause simplifiedClauseCopy;
        simplifiedClauseCopy.reserve(simplifiedClause.size());
        for (auto &simplifiedLiteral : simplifiedClause) {
            simplifiedClauseCopy.push_back(make_shared<SimplifiedLiteral>(simplifiedLiteral->getIsNegated(),
                                                                          simplifiedLiteral->getPredicateName(), simplifiedLiteral->getArguments()));
        }
        simplifiedClauseFormCopy.push_back(simplifiedClauseCopy);
    }
    return make_shared<SimplifiedClauseForm>(simplifiedClauseFormCopy);
}
std::unordered_set<std::string> SimplifiedClauseForm::unionAllArgumentsWith(const unordered_set<std::string>& terms) const {
    vector <std::string> resultVector;
    set_union(allArguments.begin(), allArguments.end(), terms.begin(), terms.end(),
    back_inserter(resultVector));
    unordered_set<std::string> result(resultVector.begin(), resultVector.end());
    return result;
}
unordered_set<std::string> SimplifiedClauseForm::makeVariableNamesUniquePerClause(const unordered_set<std::string>& variables) {
    unordered_set<string> soFar;
    for (auto &simplifiedClause : simplifiedClauseForm) {
        unordered_map<string, string> localSubstitution;
        for (auto &simplifiedLiteral : simplifiedClause) {
            auto args = simplifiedLiteral -> getAllVariablesAndConstants();
            auto vars = unionAllArgumentsWith()
            for (auto &arg : args) {
                if (soFar.find(arg) != soFar.end()) {

                }
            }
        }
    }
}

}; // namespace utils
