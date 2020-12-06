//
// Created by Patrick Sava on 12/2/2020.
//

#include "clause.h"
#include "ad_hoc_templated.h"

using namespace std;

namespace utils {
std::shared_ptr<Clause> Clause::createDeepCopy() {
    return std::shared_ptr<Clause>(shared_from_this());
}
std::unordered_set<std::string> Clause::getAllVariables() {
    unordered_set<string> result;
    for (auto &literal : clause) {
        AdHocTemplated<string>::unionIterablesUnorderedSetInPlace(literal->getAllVariables(), result, result);
    }
    return result;
}
void Clause::applySubstitution(const pair<std::string, std::shared_ptr<Term>>& mapping) {
    for (auto &literal : clause) {
        literal->applySubstitution(mapping);
    }
}
std::map<std::pair<std::string, bool>, int> Clause::getAllLiterals() {
    map<pair<string, bool>, int> accumulator;
    for(auto &literal : clause) {
        accumulator[literal->getLiteral()] += 1;
    }
    return accumulator;
}
std::string Clause::getString() const {
    string result;
    for (int index = 0; index < (int)clause.size(); ++ index) {
        result += clause[index]->getString();
        if (index + 1 != (int)clause.size()) {
            result += "^";
        }
    }
    return result;
}


};