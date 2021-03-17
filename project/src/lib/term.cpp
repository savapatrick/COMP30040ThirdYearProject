//
// Created by Patrick Sava on 12/2/2020.
//

#include "term.h"
#include <cassert>
#include <queue>

using namespace std;

namespace {
void getAllVariablesInOrder(const shared_ptr<const utils::Term>& node, vector<string>& variablesInOrder) {
    if(node->getTermType() == utils::VARIABLE) {
        variablesInOrder.push_back(node->getTermName());
    } else {
        for(auto& argument : node->getArguments()) { getAllVariablesInOrder(argument, variablesInOrder); }
    }
}
} // namespace

namespace utils {
bool Term::equals(const std::shared_ptr<Term>& other) {
    return this->getString() == other->getString();
}

bool Term::containsTerm(const string& name) const {
    queue<shared_ptr<const Term>> queueTerms;
    queueTerms.push(shared_from_this());

    while(!queueTerms.empty()) {
        auto elem = queueTerms.front();
        queueTerms.pop();

        if(elem->termName == name) {
            return true;
        }
        for(auto& neighbour : elem->arguments) { queueTerms.push(neighbour); }
    }
    return false;
}

variant<bool, pair<string, shared_ptr<Term>>>
Term::findPartialSubstitution(const shared_ptr<Term>& first, const shared_ptr<Term>& second) const {
    if(first != second and !first->equals(second)) {
        if(first->termType == TermType::CONSTANT) {
            return false;
        } else if(first->termType == TermType::VARIABLE) {
            if(second->containsTerm(first->termName)) {
                return false;
            } else {
                return make_pair(first->termName, second);
            }
        } else if(first->termType == TermType::FUNCTION) {
            if(second->termType == TermType::FUNCTION) {
                if(first->termName != second->termName) {
                    return false;
                } else {
                    for(int index = 0; index < (int)first->arguments.size(); ++index) {
                        auto res = findPartialSubstitution(first->arguments[index], second->arguments[index]);
                        if(res.index() == 0 and get<0>(res)) {
                            continue;
                        }
                        return res;
                    }
                    return true;
                }
            }
            return false;
        }
        return false;
    }
    return true;
}

void Term::applySubstitution(const pair<string, shared_ptr<Term>>& substitution) {
    clearCache();
    queue<shared_ptr<Term>> queueForTerm;
    if(termName == substitution.first) {
        auto which = substitution.second;
        termType   = which->termType;
        termName   = which->termName;
        arguments  = which->arguments;
        return;
    }
    queueForTerm.push(shared_from_this());
    while(!queueForTerm.empty()) {
        auto& frontTerm = queueForTerm.front();
        queueForTerm.pop();

        for(auto& arg : frontTerm->arguments) {
            if(arg->termName == substitution.first) {
                arg = substitution.second;
                continue;
            }
            queueForTerm.push(arg);
        }
    }
}

std::variant<bool, std::pair<std::string, std::shared_ptr<Term>>> Term::augmentUnification(const std::shared_ptr<Term>& other) {
    if(this->equals(other)) {
        return true;
    }
    clearCache();
    // first
    auto attempt = findPartialSubstitution(shared_from_this(), other);
    if(attempt.index() == 0) {
        if(!get<0>(attempt)) {
            // second
            attempt = findPartialSubstitution(other, shared_from_this());
            if(attempt.index() == 0) {
                if(!get<0>(attempt)) {
                    return false;
                }
            } else if(attempt.index()) {
                return get<1>(attempt);
            }
        }
    } else {
        return get<1>(attempt);
    }
    return true;
}

std::shared_ptr<Term> Term::createDeepCopy() {
    return make_shared<Term>(shared_from_this());
}

bool Term::hasNestedFunctions() const {
    queue<shared_ptr<const Term>> queueTerms;
    queueTerms.push(shared_from_this());
    while(!queueTerms.empty()) {
        auto& frontTerm = queueTerms.front();
        queueTerms.pop();

        if(termType == TermType::FUNCTION) {
            for(auto& neighbour : frontTerm->arguments) {
                if(neighbour->termType == TermType::FUNCTION) {
                    return true;
                }
            }
        }

        for(auto& neighbour : frontTerm->arguments) { queueTerms.push(neighbour); }
    }
    return false;
}

std::unordered_set<std::string> Term::getAllVariables() const {
    std::unordered_set<std::string> result;
    queue<shared_ptr<const Term>> queueTerms;
    queueTerms.push(shared_from_this());
    while(!queueTerms.empty()) {
        auto& frontTerm = queueTerms.front();
        queueTerms.pop();

        if(frontTerm->termType == TermType::VARIABLE) {
            result.insert(frontTerm->termName);
        }

        for(auto& neighbour : frontTerm->arguments) { queueTerms.push(neighbour); }
    }
    return result;
}

std::string Term::preOrderTraversal(const std::shared_ptr<const Term>& node,
const unordered_map<std::string, std::string>& substitution) const {
    string result;
    if(!substitution.empty() and node->termType == VARIABLE) {
        if(substitution.find(node->termName) == substitution.end()) {
            throw std::invalid_argument("the substitution given does not include all the variables in the literal");
        }
        result += substitution.at(node->termName);
    } else {
        result += node->termName;
    }
    if(node->termType == FUNCTION) {
        result += "(";
    }
    for(int index = 0; index < (int)node->arguments.size(); ++index) {
        result += preOrderTraversal(node->arguments[index], substitution);
        if(index + 1 != (int)node->arguments.size()) {
            result += ",";
        }
    }
    if(node->termType == FUNCTION) {
        result += ")";
    }
    return result;
}

std::string Term::getString() {
    if(!cachedGetString.empty()) {
        return cachedGetString;
    }
    unordered_map<string, string> emptySubstitution;
    cachedGetString = preOrderTraversal(shared_from_this(), emptySubstitution);
    return cachedGetString;
}

std::string Term::getStringWithoutVariableNames() {
    if(!cachedGetStringWithoutVariableNames.empty()) {
        return cachedGetStringWithoutVariableNames;
    }
    unordered_map<string, string> substitution;
    auto allVariables = getAllVariables();
    for(auto& variable : allVariables) { substitution[variable] = "_v_var"; }
    cachedGetStringWithoutVariableNames = preOrderTraversal(shared_from_this(), substitution);
    return cachedGetStringWithoutVariableNames;
}

std::string Term::getHash(const unordered_map<std::string, std::string>& substitution) const {
    return preOrderTraversal(shared_from_this(), substitution);
}

void Term::applySubstitution(const pair<std::string, std::string>& substitution) {
    clearCache();
    queue<shared_ptr<Term>> queueForTerm;
    auto which = make_shared<Term>(substitution.second);
    if(termName == substitution.first) {
        termType  = which->termType;
        termName  = which->termName;
        arguments = which->arguments;
        return;
    }
    queueForTerm.push(shared_from_this());
    while(!queueForTerm.empty()) {
        auto& frontTerm = queueForTerm.front();
        queueForTerm.pop();

        for(auto& arg : frontTerm->arguments) {
            if(arg->termName == substitution.first) {
                arg = which;
                continue;
            }
            queueForTerm.push(arg);
        }
    }
}
void Term::renameFunction(const pair<std::string, std::string>& substitution) {
    clearCache();
    queue<shared_ptr<Term>> queueForTerm;
    if(termName == substitution.first) {
        termName = substitution.second;
    }
    queueForTerm.push(shared_from_this());
    while(!queueForTerm.empty()) {
        auto& frontTerm = queueForTerm.front();
        queueForTerm.pop();

        for(auto& arg : arguments) {
            if(arg->termName == substitution.first) {
                arg->termName = substitution.second;
            }
            queueForTerm.push(arg);
        }
    }
}

void Term::getDepths(const shared_ptr<const Term>& node, unordered_map<std::string, int>& soFar, int currentDepth, int& maxDepth) const {
    maxDepth = max(maxDepth, currentDepth);
    if(node->termType == TermType::VARIABLE) {
        soFar[node->termName] = max(soFar[node->termName], currentDepth);
        return;
    }
    for(auto& neighbour : node->arguments) { getDepths(neighbour, soFar, currentDepth + 1, maxDepth); }
}

std::pair<int, std::unordered_map<std::string, int>> Term::getDepths() const {
    unordered_map<std::string, int> depths;
    int maxDepth = 0;
    getDepths(shared_from_this(), depths, 0, maxDepth);
    return { maxDepth, depths };
}

std::vector<std::string> Term::getAllVariablesInOrder() const {
    vector<string> allVariablesInOrder;
    ::getAllVariablesInOrder(shared_from_this(), allVariablesInOrder);
    return allVariablesInOrder;
}
const string& Term::getTermName() const {
    return termName;
}
TermType Term::getTermType() const {
    return termType;
}
const vector<std::shared_ptr<Term>>& Term::getArguments() const {
    return arguments;
}
void Term::clearCache() {
    cachedGetString.clear();
    cachedGetStringWithoutVariableNames.clear();
}

}; // namespace utils