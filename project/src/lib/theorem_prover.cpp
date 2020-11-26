//
// Created by Patrick Sava on 11/25/2020.
//

#include "theorem_prover.h"
#include <algorithm>
#include <map>

using namespace std;
namespace utils {
bool TheoremProver::hasLiteralAndNegatedLiteralInClause(const utils::ClauseForm::Clause& clause) {
    map<DualHashASCII::HashType, int> info; // 1 negative, 2 positive, 3 both negative and positive
    for(auto& literal : clause) {
        if(literal->getIsNegated()) {
            info[DualHashASCII::getHashPredicateName(literal)] ^= 1;
        } else {
            info[DualHashASCII::getHashPredicateName(literal)] ^= 2;
        }
    }
    for(auto& keyValue : info) {
        if(keyValue.second == 3) {
            return true;
        }
    }
    return false;
}
bool TheoremProver::checkForLiteralAndNegatedLiteralInClauses() {
    return all_of(begin(clauseForm), end(clauseForm),
    [&](const ClauseForm::Clause& clause) -> bool { return !hasLiteralAndNegatedLiteralInClause(clause); });
}
bool TheoremProver::applyFactoringOnClauseIfPossible(ClauseForm::Clause* result, const ClauseForm::Clause& clause) {
    bool changed = false;
    map<DualHashASCII::HashType, shared_ptr<Literal>> literals;
    for(auto& literal : clause) {
        auto literalHash = DualHashASCII::getHash(literal);
        if(literals.find(literalHash) != literals.end()) {
            // it already exists
            changed = true;
        } else {
            literals[literalHash] = literal;
        }
    }
    result->clear();
    result->reserve(literals.size());
    for(auto& info : literals) { result->push_back(info.second); }
    return changed;
}
bool TheoremProver::applyFactoringOnClausesIfPossible() {
    vector<ClauseForm::Clause> newClauseForm;
    newClauseForm.reserve(clauseForm.size());
    bool changed = false;
    for(auto& clause : clauseForm) {
        ClauseForm::Clause newClause;
        if(applyFactoringOnClauseIfPossible(&newClause, clause)) {
            newClauseForm.push_back(newClause);
            changed = true;
        } else {
            newClauseForm.push_back(clause);
        }
    }
    if(changed) {
        clauseForm = newClauseForm;
    }
    return changed;
}
map<DualHashASCII::HashType, pair<bool, bool>> TheoremProver::getLiteralsMap(const ClauseForm::Clause& first) const {
    map<DualHashASCII::HashType, pair<bool, bool>> literalsFirst;
    for(auto& literal : first) {
        pair<bool, bool> aux(false, false);
        auto literalHash = DualHashASCII::getHashPredicateName(literal);
        if(literalsFirst.find(literalHash) != literalsFirst.end()) {
            aux = literalsFirst[literalHash];
        }
        if(literal->getIsNegated()) {
            literalsFirst[literalHash] = { aux.first, true };
        } else {
            literalsFirst[literalHash] = { true, aux.second };
        }
    }
    return literalsFirst;
}
map<DualHashASCII::HashType, shared_ptr<Literal>> TheoremProver::getLiteralsInstances(const ClauseForm::Clause& first) const {
    map<DualHashASCII::HashType, shared_ptr<Literal>> literalInstances;
    for(auto& literal : first) { literalInstances[DualHashASCII::getHash(literal)] = literal; }
    return literalInstances;
}
std::pair<bool, std::pair<ClauseForm::Clause, ClauseForm::Clause>>
TheoremProver::applyResolution(const ClauseForm::Clause& first, const ClauseForm::Clause& second) {
    // This method assumes that none of the clauses have a literal and its complement in the same clause
    auto literalsInstancesFirst  = getLiteralsInstances(first);
    auto literalsInstancesSecond = getLiteralsInstances(second);
    auto literalsFirst           = getLiteralsMap(first);
    auto literalsSecond          = getLiteralsMap(second);
    bool isReduced               = false;
    for(auto& elem : literalsFirst) {
        if(literalsSecond.find(elem.first) != literalsSecond.end()) {
            if((elem.second.first and literalsSecond[elem.first].second) or
            (elem.second.second and literalsSecond[elem.first].first)) {
                literalsFirst.erase(literalsFirst.find(elem.first));
                literalsSecond.erase(literalsSecond.find(elem.first));
                isReduced = true;
            }
        }
    }
    vector<shared_ptr<Literal>> resultFirst;
    transform(literalsInstancesFirst.begin(), literalsInstancesFirst.end(), back_inserter(resultFirst),
    [](auto& keyValue) { return keyValue.second; });
    vector<shared_ptr<Literal>> resultSecond;
    transform(literalsInstancesSecond.begin(), literalsInstancesSecond.end(), back_inserter(resultSecond),
    [](auto& keyValue) { return keyValue.second; });
    return { isReduced, { resultFirst, resultSecond } };
}
}; // namespace utils
