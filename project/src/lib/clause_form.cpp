//
// Created by Patrick Sava on 12/2/2020.
//

#include "clause_form.h"
#include "ad_hoc_templated.h"
#include "random_factory.h"

using namespace std;
namespace utils {

void ClauseForm::makeVariableNamesUniquePerClause() {
    unordered_set<string> soFar;
    for(auto& clause : clauseForm) {
        auto allVariables = clause->getAllVariables();
        unordered_set<string> localVariables;
        for(auto& variable : allVariables) {
            if(soFar.find(variable) != soFar.end()) {
                auto substitution = make_pair(variable, RandomFactory::getRandomTermOrFunctionName(allVariableNames));
                clause->applySubstitution(substitution);
                localVariables.insert(substitution.second);
            } else {
                localVariables.insert(variable);
            }
        }
        AdHocTemplated<string>::unionIterablesUnorderedSetInPlace(soFar, localVariables, allVariableNames);
    }
}
}; // namespace utils