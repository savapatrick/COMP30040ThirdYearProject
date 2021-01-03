//
// Created by Patrick Sava on 12/2/2020.
//

#include "clause.h"
#include "ad_hoc_templated.h"
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
std::map<std::pair<std::string, bool>, int> Clause::getAllLiterals() const {
    map<pair<string, bool>, int> accumulator;
    for(auto& literal : clause) { accumulator[literal->getLiteral()] += 1; }
    return accumulator;
}
std::string Clause::getString() const {
    string result;
    vector <string> literals;
    literals.reserve(clause.size());
    for(const auto & literal : clause) {
        literals.push_back(literal->getString());
    }
    sort(literals.begin(), literals.end());
    for (int index = 0; index < (int)literals.size(); ++ index) {
        result += literals[index];
        if(index + 1 != (int)clause.size()) {
            result += " | ";
        }
    }
    return result;
}
void Clause::applySubstitution(const pair<std::string, std::string>& mapping) {
    for(auto& literal : clause) { literal->applySubstitution(mapping); }
}
void Clause::renameFunction(const pair<std::string, std::string>& mapping) {
    for(auto& literal : clause) { literal->renameFunction(mapping); }
}


}; // namespace utils