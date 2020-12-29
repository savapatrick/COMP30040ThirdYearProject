//
// Created by Patrick Sava on 11/25/2020.
//

#include "theorem_prover.h"

using namespace std;
namespace utils {

bool TheoremProver::isTautology(std::shared_ptr<Clause>& clause) {
    unordered_map<string, vector<shared_ptr<Literal>>> literals;
    for(auto& currentLiteral : clause->clause) {
        for(auto& literal : literals[currentLiteral->predicateName]) {
            if(currentLiteral->equalsWithoutSign(literal)) {
                if(currentLiteral->isNegated != literal->isNegated) {
                    return true;
                }
            }
        }
        literals[currentLiteral->predicateName].push_back(currentLiteral);
    }
    return false;
}

}; // namespace utils
