//
// Created by Patrick Sava on 11/18/2020.
//

#include "simplified_clause_form.h"
#include "ad_hoc_templated.h"
#include "operators.h"
#include "random_factory.h"
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
    for(auto& simplifiedClause : simplifiedClauseForm) {
        SimplifiedClauseForm::SimplifiedClause simplifiedClauseCopy;
        simplifiedClauseCopy.reserve(simplifiedClause.size());
        for(auto& simplifiedLiteral : simplifiedClause) {
            simplifiedClauseCopy.push_back(make_shared<SimplifiedLiteral>(simplifiedLiteral->getIsNegated(),
            simplifiedLiteral->getPredicateName(), simplifiedLiteral->getArguments()));
        }
        simplifiedClauseFormCopy.push_back(simplifiedClauseCopy);
    }
    return make_shared<SimplifiedClauseForm>(simplifiedClauseFormCopy);
}

unordered_set<std::string> SimplifiedClauseForm::makeVariableNamesUniquePerClause(const unordered_set<std::string>& variables) {
    unordered_set<string> soFar;
    for(auto& simplifiedClause : simplifiedClauseForm) {
        unordered_map<string, string> localSubstitution;
        vector<string> allVarsForClause;
        for(auto& simplifiedLiteral : simplifiedClause) {
            auto args = simplifiedLiteral->getAllVariablesAndConstants();
            auto vars = AdHocTemplated<std::string>::unionIterablesVector(args, variables);
            for(auto& var : vars) {
                if(soFar.find(var) != soFar.end()) {
                    if(localSubstitution.find(var) == localSubstitution.end()) {
                        localSubstitution[var] = RandomFactory::getRandomTermOrFunctionName(allArguments);
                    }
                } else {
                    allVarsForClause.push_back(var);
                }
            }
        }
        for(auto& simplifiedLiteral : simplifiedClause) { simplifiedLiteral->simpleSubstitution(localSubstitution); }
        for(auto& keyValue : localSubstitution) { allVarsForClause.push_back(keyValue.second); }
        for(auto& variable : allVarsForClause) { soFar.insert(variable); }
    }
    return soFar;
}

}; // namespace utils
