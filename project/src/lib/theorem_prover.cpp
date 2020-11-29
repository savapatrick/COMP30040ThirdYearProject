//
// Created by Patrick Sava on 11/25/2020.
//

#include "theorem_prover.h"
#include <algorithm>
#include <map>

using namespace std;
namespace utils {
bool TheoremProver::hasLiteralAndNegatedLiteralInClause(const utils::SimplifiedClauseForm::SimplifiedClause& clause) {
    map<DualHashASCII::HashType, int> info; // 1 negative, 2 positive, 3 both negative and positive
    for(auto& simplifiedLiteral : clause) {
        if(simplifiedLiteral->getIsNegated()) {
            info[DualHashASCII::getHashPredicateName(simplifiedLiteral)] ^= 1;
        } else {
            info[DualHashASCII::getHashPredicateName(simplifiedLiteral)] ^= 2;
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
    return all_of(begin(simplifiedClauseForm), end(simplifiedClauseForm),
    [&](const SimplifiedClauseForm::SimplifiedClause& clause) -> bool {
        return !hasLiteralAndNegatedLiteralInClause(clause);
    });
}
bool TheoremProver::applyFactoringOnClauseIfPossible(SimplifiedClauseForm::SimplifiedClause* result,
const SimplifiedClauseForm::SimplifiedClause& clause) {
    bool changed = false;
    map<DualHashASCII::HashType, shared_ptr<SimplifiedLiteral>> literals;
    for(auto& literal : clause) {
        auto simplifiedLiteralHash = DualHashASCII::getHash(literal);
        if(literals.find(simplifiedLiteralHash) != literals.end()) {
            // it already exists
            changed = true;
        } else {
            literals[simplifiedLiteralHash] = literal;
        }
    }
    result->clear();
    result->reserve(literals.size());
    for(auto& info : literals) { result->push_back(info.second); }
    return changed;
}
bool TheoremProver::applyFactoringOnClausesIfPossible() {
    vector<SimplifiedClauseForm::SimplifiedClause> newClauseForm;
    newClauseForm.reserve(simplifiedClauseForm.size());
    bool changed = false;
    for(auto& clause : simplifiedClauseForm) {
        SimplifiedClauseForm::SimplifiedClause newClause;
        if(applyFactoringOnClauseIfPossible(&newClause, clause)) {
            newClauseForm.push_back(newClause);
            changed = true;
        } else {
            newClauseForm.push_back(clause);
        }
    }
    if(changed) {
        simplifiedClauseForm = newClauseForm;
    }
    return changed;
}
map<DualHashASCII::HashType, pair<bool, bool>> TheoremProver::getLiteralsMap(const SimplifiedClauseForm::SimplifiedClause& first) const {
    map<DualHashASCII::HashType, pair<bool, bool>> literalsFirst;
    for(auto& literal : first) {
        pair<bool, bool> aux(false, false);
        auto simplifiedLiteralHash = DualHashASCII::getHashPredicateName(literal);
        if(literalsFirst.find(simplifiedLiteralHash) != literalsFirst.end()) {
            aux = literalsFirst[simplifiedLiteralHash];
        }
        if(literal->getIsNegated()) {
            literalsFirst[simplifiedLiteralHash] = { aux.first, true };
        } else {
            literalsFirst[simplifiedLiteralHash] = { true, aux.second };
        }
    }
    return literalsFirst;
}
map<DualHashASCII::HashType, shared_ptr<SimplifiedLiteral>> TheoremProver::getLiteralsInstances(
const SimplifiedClauseForm::SimplifiedClause& first) const {
    map<DualHashASCII::HashType, shared_ptr<SimplifiedLiteral>> literalInstances;
    for(auto& literal : first) { literalInstances[DualHashASCII::getHash(literal)] = literal; }
    return literalInstances;
}
std::pair<bool, std::pair<SimplifiedClauseForm::SimplifiedClause, SimplifiedClauseForm::SimplifiedClause>>
TheoremProver::applyResolution(const SimplifiedClauseForm::SimplifiedClause& first,
const SimplifiedClauseForm::SimplifiedClause& second) {
    // This method assumes that none of the clauses have a simplifiedLiteral and its complement in the same clause
    auto simplifiedLiteralsInstancesFirst  = getLiteralsInstances(first);
    auto simplifiedLiteralsInstancesSecond = getLiteralsInstances(second);
    auto simplifiedLiteralsFirst           = getLiteralsMap(first);
    auto simplifiedLiteralsSecond          = getLiteralsMap(second);
    bool isReduced                         = false;
    for(auto& elem : simplifiedLiteralsFirst) {
        if(simplifiedLiteralsSecond.find(elem.first) != simplifiedLiteralsSecond.end()) {
            if((elem.second.first and simplifiedLiteralsSecond[elem.first].second) or
            (elem.second.second and simplifiedLiteralsSecond[elem.first].first)) {
                simplifiedLiteralsFirst.erase(simplifiedLiteralsFirst.find(elem.first));
                simplifiedLiteralsSecond.erase(simplifiedLiteralsSecond.find(elem.first));
                isReduced = true;
            }
        }
    }
    vector<shared_ptr<SimplifiedLiteral>> resultFirst;
    transform(simplifiedLiteralsInstancesFirst.begin(), simplifiedLiteralsInstancesFirst.end(),
    back_inserter(resultFirst), [](auto& keyValue) { return keyValue.second; });
    vector<shared_ptr<SimplifiedLiteral>> resultSecond;
    transform(simplifiedLiteralsInstancesSecond.begin(), simplifiedLiteralsInstancesSecond.end(),
    back_inserter(resultSecond), [](auto& keyValue) { return keyValue.second; });
    return { isReduced, { resultFirst, resultSecond } };
}
}; // namespace utils
