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

void ClauseForm::renameFunction(const std::pair<std::string, std::string>& mapping) {
    for(auto& clause : clauseForm) { clause->renameFunction(mapping); }
}

void ClauseForm::applySubstitution(const std::pair<std::string, std::string>& mapping) {
    for(auto& clause : clauseForm) { clause->applySubstitution(mapping); }
}

void ClauseForm::renameTerms(std::shared_ptr<ClauseForm>& other,
std::unordered_set<std::string>& _allTermNames,
std::unordered_set<std::string>& _allTermNamesOther) {
    vector<pair<string, string>> namesToBeChanged;
    for(auto& termName : _allTermNamesOther) {
        if(_allTermNames.find(termName) != _allTermNames.end()) {
            namesToBeChanged.emplace_back(termName, "");
        } else {
            _allTermNames.insert(termName);
        }
    }
    for(auto& termName : namesToBeChanged) {
        termName.second = RandomFactory::getRandomTermOrFunctionName(_allTermNames);
        while(_allTermNamesOther.find(termName.second) != _allTermNamesOther.end()) {
            _allTermNames.erase(_allTermNames.find(termName.second));
            termName.second = RandomFactory::getRandomTermOrFunctionName(_allTermNames);
        }
        _allTermNamesOther.insert(termName.second);
    }
    for(auto& termName : namesToBeChanged) { other->renameFunction(termName); }
}

void ClauseForm::merge(std::shared_ptr<ClauseForm>& other) {
    renameTerms(other, allFunctionNames, other->allFunctionNames);
    renameTerms(other, allVariableNames, other->allVariableNames);
    for(auto& constantName : other->allConstantNames) { allConstantNames.insert(constantName); }
    for(auto& clause : other->clauseForm) { clauseForm.push_back(clause); }
}

}; // namespace utils