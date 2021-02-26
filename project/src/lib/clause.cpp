//
// Created by Patrick Sava on 12/2/2020.
//

#include "clause.h"
#include "ad_hoc_templated.h"
#include "random_factory.h"
#include <algorithm>

using namespace std;

namespace utils {
std::shared_ptr<Clause> Clause::createDeepCopy() {
    return std::make_shared<Clause>(shared_from_this());
}
bool Clause::hasNestedFunctions() {
    for(auto& literal : clause) {
        if(literal->hasNestedFunctions()) {
            return true;
        }
    }
    return false;
}
int Clause::getHighestNumberOfVariablesPerLiteral() {
    int answer = 0;
    for(auto& literal : clause) { answer = max(answer, static_cast<int>(literal->getAllVariables().size())); }
    return answer;
}
std::unordered_set<std::string> Clause::getAllVariables() {
    unordered_set<string> result;
    for(auto& literal : clause) {
        AdHocTemplated<string>::unionIterablesUnorderedSetInPlace(literal->getAllVariables(), result, result);
    }
    return result;
}
void Clause::applySubstitution(const pair<std::string, std::shared_ptr<Term>>& mapping) {
    for(auto& literal : clause) { literal->applySubstitution(mapping); }
}
std::map<std::pair<std::string, bool>, int> Clause::getLiteralsAndCount() const {
    map<pair<string, bool>, int> accumulator;
    for(auto& literal : clause) { accumulator[literal->getLiteral()] += 1; }
    return accumulator;
}
std::string Clause::getString() const {
    string result;
    vector<string> literals;
    literals.reserve(clause.size());
    for(const auto& literal : clause) { literals.push_back(literal->getString()); }
    sort(literals.begin(), literals.end());
    for(int index = 0; index < (int)literals.size(); ++index) {
        result += literals[index];
        if(index + 1 != (int)clause.size()) {
            result += " | ";
        }
    }
    return result;
}

std::unordered_set<std::string> Clause::getHashSet() const {
    vector<pair<string, shared_ptr<Literal>>> literals;
    literals.reserve(clause.size());
    for(const auto& literal : clause) { literals.emplace_back(literal->getStringWithoutVariableNames(), literal); }
    sort(literals.begin(), literals.end());
    unordered_map<string, string> substitution;
    int currentLabel = 0;
    unordered_set<string> result;
    for(const auto& literal : literals) {
        auto variablesInOrder = literal.second->getAllVariablesInOrder();
        for(auto& currentVariable : variablesInOrder) {
            if(substitution.find(currentVariable) == substitution.end()) {
                currentLabel += 1;
                substitution[currentVariable] = "_v_" + to_string(currentLabel);
            }
        }
        result.insert(literal.second->getHash(substitution));
    }
    return result;
}

std::string Clause::getHash() const {
    auto hashSet = getHashSet();
    string result;
    for(auto& current : hashSet) {
        result += current;
        result += "|";
    }
    if(!result.empty()) {
        result.pop_back();
    }
    return result;
}

void Clause::applySubstitution(const pair<std::string, std::string>& mapping) {
    for(auto& literal : clause) { literal->applySubstitution(mapping); }
}
void Clause::renameFunction(const pair<std::string, std::string>& mapping) {
    for(auto& literal : clause) { literal->renameFunction(mapping); }
}
const std::vector<std::shared_ptr<Literal>>& Clause::getLiterals() const {
    return clause;
}
void Clause::disjointifyVariables(shared_ptr<Clause>& other) {
    auto allOtherVariables = other->getAllVariables();
    auto allVariables      = this->getAllVariables();
    for(auto& variable : allOtherVariables) {
        if(allVariables.find(variable) != allVariables.end()) {
            auto substitution = make_pair(variable, RandomFactory::getRandomVariableName(allVariables));
            this->applySubstitution(substitution);
        }
    }
}


}; // namespace utils