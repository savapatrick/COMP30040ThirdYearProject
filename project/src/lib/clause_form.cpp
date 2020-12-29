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
                auto substitution = make_pair(variable, RandomFactory::getRandomVariableName(allVariableNames));
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
std::unordered_set<std::string>& _allTermNamesOther,
std::unordered_set<std::string>& forbiddenOne,
std::unordered_set<std::string>& forbiddenTwo,
const bool isFunctionRenaming) {
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
        while(_allTermNamesOther.find(termName.second) != _allTermNamesOther.end() ||
        forbiddenOne.find(termName.second) != forbiddenOne.end() || forbiddenTwo.find(termName.second) != forbiddenTwo.end()) {
            _allTermNames.erase(_allTermNames.find(termName.second));
            termName.second = RandomFactory::getRandomTermOrFunctionName(_allTermNames);
        }
        _allTermNamesOther.insert(termName.second);
        _allTermNames.insert(termName.second);
    }
    for(auto& termName : namesToBeChanged) {
        if(isFunctionRenaming) {
            other->renameFunction(termName);
        } else {
            other->applySubstitution(termName);
        }
    }
}

void ClauseForm::merge(std::shared_ptr<ClauseForm>& other) {
    renameTerms(other, allFunctionNames, other->allFunctionNames, allVariableNames, allConstantNames, true);
    renameTerms(other, allVariableNames, other->allVariableNames, allFunctionNames, allConstantNames, false);
    for(auto& constantName : other->allConstantNames) { allConstantNames.insert(constantName); }
    for(auto& clause : other->clauseForm) { clauseForm.push_back(clause); }
}

std::string ClauseForm::getString() const {
    string result;
    result += "{";
    for(int index = 0; index < (int)clauseForm.size(); ++index) {
        result += clauseForm[index]->getString();
        if(index + 1 != (int)clauseForm.size()) {
            result += ", ";
        }
    }
    result += "}";
    return result;
}

std::string ClauseForm::getStringWithIndex() const {
    string result;
    for(int index = 0; index < (int)clauseForm.size(); ++index) {
        result += to_string(index) + " " + clauseForm[index]->getString() + "\n";
    }
    return result;
}

bool ClauseForm::isTwoVariableFragment() {
    for (auto& clause: clauseForm) {
        if (clause->hasNestedFunctions() or clause->getAllVariables().size() > 2) {
            return false;
        }
    }
    return true;
}

}; // namespace utils