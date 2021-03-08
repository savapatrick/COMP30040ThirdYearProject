//
// Created by Patrick Sava on 12/2/2020.
//

#include "clause_form.h"
#include "ad_hoc_templated.h"
#include "operators.h"
#include "random_factory.h"
#include "reducer.h"
#include <algorithm>

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

std::string ClauseForm::getStringWithIndex(const std::unordered_map<int, int>& isDeleted) const {
    string result;
    for(int index = 0; index < (int)clauseForm.size(); ++index) {
        if(isDeleted.find(index) == isDeleted.end()) {
            result += to_string(index) + " " + clauseForm[index]->getString() + "\n";
        }
    }
    return result;
}

bool ClauseForm::isTwoVariableFragment() {
    for(auto& clause : clauseForm) {
        if(clause->hasNestedFunctions() or clause->getAllVariables().size() > 2) {
            return false;
        }
    }
    return true;
}

bool ClauseForm::containsEquality() {
    return getString().find("Equality(") != string::npos;
}

void ClauseForm::copyToThis(const std::shared_ptr<ClauseForm>& other) {
    clauseForm = other->clauseForm;
    allFunctionNames = other->allFunctionNames;
    allVariableNames = other->allVariableNames;
    allConstantNames = other->allConstantNames;
}

void ClauseForm::enforcePureTwoVariableFragment() {
    const vector <string> newVariables({"_v_x, _v_y"});
    bool hasTwo = false;
    for (auto &clause: clauseForm) {
        auto clauseVariables = clause->getAllVariables();
        if (clauseVariables.size() > 2) {
            throw std::logic_error("The clause " + clause->getString() + "contains more than two variables!");
        }
        if (clauseVariables.size() == 2) {
            if (!clause->containsEquality()) {
                throw std::logic_error("Only clauses containing equality should be in two variables at this point!; clause " + clause->getString() + " violates this!");
            }
            hasTwo = true;
        }
        // set because otherwise we would mess up the order of the variables
        set<string> currentNewVariables(newVariables.begin(), newVariables.end());
        for (auto &newVariable : newVariables) {
            if(currentNewVariables.find(newVariable) != currentNewVariables.end() and
            clauseVariables.find(newVariable) != clauseVariables.end()) {
                currentNewVariables.erase(newVariable);
                clauseVariables.erase(newVariable);
            }
        }
        vector<string> clauseVariablesArray(clauseVariables.begin(), clauseVariables.end());
        vector<string> newVariablesArray(currentNewVariables.begin(), currentNewVariables.end());
        for (int index = 0; index < (int)clauseVariablesArray.size(); ++ index) {
            clause->applySubstitution({clauseVariablesArray[index], newVariablesArray[index]});
        }
    }
    allVariableNames.clear(); allVariableNames.insert(newVariables[0]);
    if (hasTwo) {
        allVariableNames.insert(newVariables[1]);
    }
}

void ClauseForm::resolveEquality() {
    Operators& operators = Operators::getInstance();
    enforcePureTwoVariableFragment();
    map <string, shared_ptr<Literal>> arityOneLiterals;
    for (auto &clause: clauseForm) {
        auto literals = clause->getLiterals();
        for (auto &literal : literals) {
            auto arityWithoutConstants = literal->getArityExcludingConstants();
            if (literal->getIsEquality()) {
                continue; // we don't process the equality
            }
            if (arityWithoutConstants == 1) {
                auto predicateString = literal->getPredicateName() + literal->getTerms();
                if (arityOneLiterals.find(predicateString) == arityOneLiterals.end()) {
                    auto predicate = literal->createDeepCopy();
                    if(predicate->getIsNegated()) {
                        predicate->negate();
                    }
                    arityOneLiterals[predicateString] = predicate;
                }
            }
            else if (arityWithoutConstants > 1) {
                throw std::logic_error("The arity of " + literal->getString() +
                                " should be 0, 1; literal: " + literal->getString());
            }
        }
    }
    string resultedClause;
    for (auto &clause : clauseForm) {
        if (clause->containsEquality()) {
            string currentClause;
            map<string, vector <string>> groups;
            const string noVariables = "<empty>";
            auto literals = clause->getLiterals();
            for (auto &literal : literals) {
                if (literal->getIsEquality()) {
                    // todo: implement here the last bit left
                }
                else {
                    auto variables = literal->getAllVariables();
                    if (variables.size() > 1) {
                        throw std::logic_error("Any literal containing more than one variable"
                                               " should be Equality at this point!");
                    }
                    else if (variables.size() == 1) {
                        groups[*variables.begin()].push_back(literal->getString());
                    }
                    else {
                        groups[noVariables].push_back(literal->getString());
                    }
                }
            }
            for (auto &bucket : groups) {
                for (auto &currentLiteral : bucket.second) {
                    if (bucket.first != noVariables) {
                        currentClause += operators.VQuantifier + bucket.first;
                    }
                    currentClause += operators.OPENEDBracket + currentLiteral + operators.CLOSEDBracket;
                    currentClause += operators.OR;
                }
            }
            if (!currentClause.empty()) {
                currentClause.pop_back();
            }
            resultedClause += operators.OPENEDBracket + currentClause + operators.CLOSEDBracket + operators.AND;
        }
        else {
            auto variables = clause->getAllVariables();
            if (variables.size() > 1) {
                throw std::logic_error("Any clause containing more than one variable "
                                       "should contain Equality as well at this point");
            }
            else if (variables.size() == 1) {
                string whichVariable = *variables.begin();
                resultedClause += operators.VQuantifier + whichVariable + operators.OPENEDBracket +
                                  clause->getString() + operators.CLOSEDBracket;
            }
            else {
                resultedClause += operators.OPENEDBracket +
                                  clause->getString() + operators.CLOSEDBracket;
            }
        }
        resultedClause += operators.AND;
    }
    if (!resultedClause.empty()) {
        resultedClause.pop_back();
    }
    ParseTree tree(resultedClause);
    Reducer reducer(tree);
    auto newClauseForm = reducer.getClauseForm();
    copyToThis(newClauseForm);
}

}; // namespace utils