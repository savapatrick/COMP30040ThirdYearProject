//
// Created by Patrick Sava on 11/25/2020.
//

#include "theorem_prover.h"
#include <string>
#include <unordered_map>

using namespace std;
namespace utils {
bool TheoremProver::hasLiteralAndNegatedLiteralInClause(const utils::ClauseForm::Clause& clause) {
    unordered_map<int, int> negated; // 1 negative, 2 positive, 3 both negative and positive
    for(auto& literal : clause) {}
}
bool TheoremProver::checkForLiteralAndNegatedLiteralInClauses() {
    for(auto& clause : clauseForm) {
        if(hasLiteralAndNegatedLiteralInClause(clause)) {
            return false;
        }
    }
    return true;
}
}; // namespace utils
