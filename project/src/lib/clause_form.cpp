//
// Created by Patrick Sava on 12/2/2020.
//

#include "clause_form.h"
#include "ad_hoc_templated.h"
#include "operators.h"
#include "random_factory.h"
#include "reducer.h"
#include <iostream>
#include <regex>

using namespace std;

namespace {
bool checkTwoColorability(const string& node, const unordered_map<string, vector<string>>& graph, unordered_map<string, bool>& colors) {
    if(colors.find(node) != colors.end()) {
        return true;
    }
    for(auto& neighbour : graph.at(node)) {
        if(colors.find(neighbour) != colors.end()) {
            if(colors.find(node) == colors.end()) {
                colors[node] = !colors[neighbour];
            } else if(colors[node] == colors[neighbour]) {
                return false;
            }
        }
    }
    if(colors.find(node) == colors.end()) {
        colors[node] = true;
    }
    for(auto& neighbour : graph.at(node)) {
        if(!checkTwoColorability(neighbour, graph, colors)) {
            return false;
        }
    }
    return true;
}
} // namespace

namespace utils {

void ClauseForm::enforceNotHaving(const string& x, const string& y) {
    if(allVariableNames.find(x) != allVariableNames.end()) {
        auto which = RandomFactory::getRandomVariableName(allVariableNames);
        while(which == y) {
            allVariableNames.erase(allVariableNames.find(which));
            which = RandomFactory::getRandomVariableName(allVariableNames);
        }
        applySubstitution({ x, which });
        allVariableNames.erase(allVariableNames.find(x));
    }
}

void ClauseForm::makeVariableNamesUniquePerClause() {
    allVariableNames.clear();
    for(auto& clause : clauseForm) {
        auto allVariables = clause->getAllVariables();
        for(auto& variable : allVariables) {
            if(allVariableNames.find(variable) != allVariableNames.end()) {
                auto substitution = make_pair(variable, RandomFactory::getRandomVariableName(allVariableNames));
                clause->applySubstitution(substitution);
            } else {
                allVariableNames.insert(variable);
            }
        }
    }
    enforceNotHaving("_v_x", "_v_y");
    enforceNotHaving("_v_y", "_v_x");
    if(allVariableNames.find("_v_x") != allVariableNames.end() or allVariableNames.find("_v_y") == allVariableNames.end()) {
        throw logic_error("[Clause form] At this point _v_x and _v_y should not occur "
                          "in the clause form.");
    }
}

void ClauseForm::renameFunction(const std::pair<std::string, std::string>& mapping) {
    for(auto& clause : clauseForm) { clause->renameFunction(mapping); }
}

void ClauseForm::applySubstitution(const std::pair<std::string, std::string>& mapping) {
    for(auto& clause : clauseForm) { clause->applySubstitution(mapping); }
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

bool ClauseForm::makeTwoVariableFragment() {
    unordered_map<string, vector<string>> graph;
    for(auto& clause : clauseForm) {
        if(clause->getMaximumNumberOfVariablesPerLiteral() > 2 or clause->hasNestedFunctions()) {
            return false;
        } else {
            auto literals = clause->getLiterals();
            for(auto& literal : literals) {
                auto vars = literal->getAllVariables();
                if(vars.size() > 2) {
                    return false;
                }
                if(vars.size() == 2) {
                    vector<string> variables(vars.begin(), vars.end());
                    graph[variables[0]].emplace_back(variables[1]);
                    graph[variables[1]].emplace_back(variables[0]);
                } else if(vars.size() == 1) {
                    if(graph.find(*vars.begin()) == graph.end()) {
                        graph[*vars.begin()] = {};
                    }
                }
            }
        }
    }
    unordered_map<string, bool> answer;
    for(auto& information : graph) {
        if(!checkTwoColorability(information.first, graph, answer)) {
            return false;
        }
    }
    if(allVariableNames.find("_v_x") != allVariableNames.end() or allVariableNames.find("_v_y") != allVariableNames.end()) {
        throw logic_error("[Two Variable Fragment Validity] At this point _v_x and _v_y should not occur "
                          "in the given clause form.");
    }
    allVariableNames.clear();
    for(auto& substitution : answer) {
        string which = substitution.second ? "_v_x" : "_v_y";
        allVariableNames.insert(which);
        applySubstitution({ substitution.first, which });
    }
    return true;
}

bool ClauseForm::containsEquality() const {
    return getString().find("Equality(") != string::npos;
}

void ClauseForm::copyToThis(const std::shared_ptr<ClauseForm>& other) {
    clauseForm       = other->clauseForm;
    allFunctionNames = other->allFunctionNames;
    allVariableNames = other->allVariableNames;
    allConstantNames = other->allConstantNames;
}

void ClauseForm::enforcePureTwoVariableFragment() {
    const vector<string> newVariables({ "_v_x", "_v_y" });
    bool hasTwo = false;
    for(auto& clause : clauseForm) {
        auto clauseVariables = clause->getAllVariables();
        if(clauseVariables.size() > 2) {
            throw std::logic_error("The clause " + clause->getString() + "contains more than two variables!");
        } else if(clauseVariables.size() == 2) {
            hasTwo = true;
        }
        // set because otherwise we would mess up the order of the variables
        set<string> currentNewVariables(newVariables.begin(), newVariables.end());
        for(auto& newVariable : newVariables) {
            if(currentNewVariables.find(newVariable) != currentNewVariables.end() and
            clauseVariables.find(newVariable) != clauseVariables.end()) {
                currentNewVariables.erase(newVariable);
                clauseVariables.erase(newVariable);
            }
        }
        vector<string> clauseVariablesArray(clauseVariables.begin(), clauseVariables.end());
        vector<string> newVariablesArray(currentNewVariables.begin(), currentNewVariables.end());
        for(int index = 0; index < (int)clauseVariablesArray.size(); ++index) {
            clause->applySubstitution({ clauseVariablesArray[index], newVariablesArray[index] });
        }
    }
    allVariableNames.clear();
    allVariableNames.insert(newVariables[0]);
    if(hasTwo) {
        allVariableNames.insert(newVariables[1]);
    }
}

void ClauseForm::resolveEquality() {
    Operators& operators = Operators::getInstance();
    enforcePureTwoVariableFragment();
    map<string, shared_ptr<Literal>> arityOneLiterals;
    for(auto& clause : clauseForm) {
        auto literals = clause->getLiterals();
        for(auto& literal : literals) {
            auto arityWithoutConstants = literal->getArityExcludingConstants();
            if(literal->getIsEquality()) {
                continue; // we don't process the equality
            }
            if(arityWithoutConstants == 1) {
                auto predicateString = literal->getPredicateName() + literal->getTerms();
                if(arityOneLiterals.find(predicateString) == arityOneLiterals.end()) {
                    auto predicate = literal->createDeepCopy();
                    if(predicate->getIsNegated()) {
                        predicate->negate();
                    }
                    arityOneLiterals[predicateString] = predicate;
                }
            } else if(arityWithoutConstants > 1) {
                throw std::logic_error("The arity of " + literal->getString() + " should be 0, 1; literal: " + literal->getString());
            }
        }
    }
    string resultedClause;
    for(auto& clause : clauseForm) {
        string currentClause;
        map<string, vector<shared_ptr<Literal>>> groups;
        const string noVariables = "<empty>";
        auto literals            = clause->getLiterals();
        for(auto& literal : literals) {
            if(literal->getIsEquality()) {
                continue; // this is going to be resolved in the next for loop
            } else {
                auto variables = literal->getAllVariables();
                if(variables.size() > 1) {
                    throw std::logic_error("Any literal containing more than one variable"
                                           " should be Equality at this point!");
                } else if(variables.size() == 1) {
                    groups[*variables.begin()].push_back(literal);
                } else {
                    groups[noVariables].push_back(literal);
                }
            }
        }
        string equalityPart;
        if(clause->containsEquality()) {
            for(auto& literal : literals) {
                if(literal->getIsEquality()) {
                    if(groups["_v_x"].empty() or groups["_v_y"].empty()) {
                        continue;
                    }
                    string lhs, rhs;
                    string lhsImplication = operators.OPENEDBracket;
                    for(auto& currentLiteral : groups["_v_x"]) {
                        lhsImplication += currentLiteral->getString() + operators.OR;
                    }
                    lhsImplication.pop_back();
                    lhsImplication.append(operators.CLOSEDBracket);
                    string rhsImplication = operators.OPENEDBracket;
                    for(auto& currentLiteral : groups["_v_y"]) {
                        auto currentLiteralDeepCopy = currentLiteral->createDeepCopy();
                        currentLiteralDeepCopy->applySubstitution({ "_v_y", "_v_x" });
                        rhsImplication += currentLiteralDeepCopy->getString() + operators.OR;
                    }
                    rhsImplication.pop_back();
                    rhsImplication.append(operators.CLOSEDBracket);
                    rhs.append(operators.VQuantifier);
                    rhs.append("_v_x");
                    rhs.append(operators.OPENEDBracket);
                    rhs.append(lhsImplication);
                    rhs.append(operators.DOUBLEImply);
                    rhs.append(rhsImplication);
                    rhs.append(operators.CLOSEDBracket);
                    vector<shared_ptr<Literal>> literalsInX;
                    literalsInX.reserve(groups["_v_x"].size());
                    for(auto& currentLiteral : groups["_v_x"]) { literalsInX.push_back(currentLiteral); }
                    auto newClauseX  = make_shared<Clause>(literalsInX);
                    auto newConstant = RandomFactory::getRandomConstantName(allConstantNames);
                    vector<string> formulas;
                    for(auto& currentPredicate : arityOneLiterals) {
                        auto allVariablesForPredicate = currentPredicate.second->getAllVariables();
                        if(allVariablesForPredicate.find("_v_x") == allVariablesForPredicate.end()) {
                            currentPredicate.second->applySubstitution({ "_v_y", "_v_x" });
                        }
                        auto predicateX = currentPredicate.second->getString();
                        currentPredicate.second->applySubstitution({ "_v_x", newConstant });
                        auto predicateConstant = currentPredicate.second->getString();
                        auto newFormula        = operators.OPENEDBracket;
                        newFormula.append(predicateX);
                        newFormula.append(operators.DOUBLEImply);
                        newFormula.append(predicateConstant);
                        newFormula.append(operators.CLOSEDBracket);
                        formulas.push_back(newFormula);
                    }
                    auto stringNegatedClauseX = operators.OPENEDBracket;
                    stringNegatedClauseX.append(operators.NOT);
                    stringNegatedClauseX.append(operators.OPENEDBracket);
                    stringNegatedClauseX.append(newClauseX->getString());
                    stringNegatedClauseX.append(operators.CLOSEDBracket);
                    stringNegatedClauseX.append(operators.CLOSEDBracket);
                    newClauseX->applySubstitution({ "_v_x", newConstant });
                    lhs = operators.OPENEDBracket;
                    lhs.append(operators.NOT);
                    lhs.append(operators.OPENEDBracket);
                    lhs.append(newClauseX->getString());
                    lhs.append(operators.CLOSEDBracket);
                    lhs.append(operators.CLOSEDBracket);
                    for(auto& formula : formulas) {
                        string implication = operators.VQuantifier;
                        implication.append("_v_x");
                        implication.append(operators.OPENEDBracket);
                        implication.append(stringNegatedClauseX);
                        implication.append(operators.IMPLY);
                        implication.append(formula);
                        implication.append(operators.CLOSEDBracket);
                        lhs.append(operators.AND);
                        lhs.append(implication);
                    }
                    string equivalence = operators.OPENEDBracket;
                    equivalence.append(lhs);
                    equivalence.append(operators.AND);
                    equivalence.append(rhs);
                    equivalence.append(operators.CLOSEDBracket);
                    equalityPart.append(equivalence);
                    equalityPart.append(operators.OR);
                }
            }
        }
        if(!equalityPart.empty()) {
            equalityPart.pop_back();
        }
        for(auto& bucket : groups) {
            for(auto& currentLiteral : bucket.second) {
                if(bucket.first != noVariables) {
                    currentClause += operators.VQuantifier + bucket.first;
                }
                currentClause += operators.OPENEDBracket + currentLiteral->getString() + operators.CLOSEDBracket;
                currentClause += operators.OR;
            }
        }
        if(!currentClause.empty()) {
            currentClause.pop_back();
        }
        if(!equalityPart.empty()) {
            currentClause.append(operators.OR);
            currentClause.append(equalityPart);
        }
        resultedClause += operators.OPENEDBracket + currentClause + operators.CLOSEDBracket;
        resultedClause += operators.AND;
    }
    if(!resultedClause.empty()) {
        resultedClause.pop_back();
    }
    unordered_set<string> allPredicates;
    for(auto& clause : clauseForm) {
        auto literals = clause->getLiterals();
        for(auto& literal : literals) { allPredicates.insert(literal->getPredicateName()); }
    }
    resultedClause = regex_replace(resultedClause, regex("Inequality"), RandomFactory::getRandomPredicateName(allPredicates));
    resultedClause = regex_replace(resultedClause, regex("_v_"), "v");
    resultedClause = regex_replace(resultedClause, regex("_c_"), "c");
    if(regex_search(resultedClause, regex("_f_"))) {
        throw std::logic_error("The intermediate representation of the given formula contains "
                               "function symbols, and that's not decidable. Please give a formula "
                               "in the Scott Normal form or try other formula which would not contain "
                               "function symbols! The intermediate representation is the following " +
        resultedClause);
    }
    cerr << resultedClause << '\n';
    cerr.flush();
    ParseTree tree(resultedClause);
    Reducer reducer(tree);
    auto newClauseForm = reducer.getClauseForm();
    if(!newClauseForm->makeTwoVariableFragment()) {
        throw logic_error("The intermediate representation of the given set of formulas in an invalid "
                          "two variable fragment: " +
        newClauseForm->getString());
    }
    copyToThis(newClauseForm);
}

}; // namespace utils