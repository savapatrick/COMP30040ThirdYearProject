//
// Created by Patrick Sava on 11/25/2020.
//

#include "basic_theorem_prover.h"
#include <algorithm>
#include <random>

using namespace std;

namespace utils {

bool BasicTheoremProver::removeDuplicates(std::shared_ptr<Clause>& clause) {
    if(clause->clause.size() == 1) {
        return false;
    }
    unordered_map<string, shared_ptr<Literal>> literals;
    for(auto& literal : clause->clause) { literals[literal->getString()] = literal; }
    outputStream << "removing duplicates from clause " + clause->getString() << "\n";
    if(literals.size() != clause->clause.size()) {
        clause->clause.clear();
        clause->clause.reserve(literals.size());
        for(auto& keyValue : literals) { clause->clause.emplace_back(keyValue.second); }
        outputStream << "it then becomes " << clause->getString() << "\n";
        return true;
    }
    return false;
}

bool BasicTheoremProver::factoringStep() {
    bool changed = false;
    std::vector<std::shared_ptr<Clause>> newClauseForm;
    for(auto& clause : clauseForm->clauseForm) {
        changed |= removeDuplicates(clause);
        newClauseForm.push_back(clause);
        if (isTautology(clause)) {
            outputStream << "clause " + clause->getString() + " is a tautology, so it's dropped\n";
            newClauseForm.pop_back();
            changed = true;
            avoid.clear();
            clausesSoFar.erase(clausesSoFar.find(clause->getString()));
            continue;;
        }
        auto unificationResult = unification->tryToUnifyTwoLiterals(clause);
        if(unificationResult.index()) {
            newClauseForm.push_back(std::get<1>(unificationResult));
            if(isTautology(newClauseForm.back()) or clausesSoFar.find(newClauseForm.back()->getString()) != clausesSoFar.end()) {
                newClauseForm.pop_back();
            }
            else {
                if (!removeDuplicates(newClauseForm.back())) {
                    newClauseForm.pop_back();
                }
                else {
                    changed = true;
                }
            }
        }
    }
    if(changed) {
        clauseForm->clauseForm = newClauseForm;
    }
    if(clauseForm->clauseForm.empty()) {
        return true;
    }
    return false;
}

bool BasicTheoremProver::run() {
    outputStream << "[basic theorem prover]\nwe have the following clauses in our initial set!\n";
    outputStream << clauseForm->getStringWithIndex();
    auto literalPredicate = [](shared_ptr<Literal>& first, shared_ptr<Literal>& second) -> bool {
        return (first->isNegated != second->isNegated) and (first->predicateName == second->predicateName);
    };
    auto resolventPredicate = [](const std::shared_ptr<Literal>& resolvedLiteral,
                              const std::vector<std::shared_ptr<Literal>>& resolvents) -> bool { return true; };
    do {
        outputData();
        int times = 0;
        if(factoringStep()) {
            outputStream << "proved by deriving the empty clause!\n";
            outputData();
            return false;
        } else {
            times += 1;
        }
        if(resolutionStep<decltype(literalPredicate), decltype(resolventPredicate)>(literalPredicate, resolventPredicate)) {
            outputStream << "proved by deriving the empty clause!\n";
            outputData();
            return false;
        } else {
            times += 1;
        }
        if(times == 2 and hot.empty()) {
            outputStream << "refuted by reaching saturation!\n";
            outputData();
            return true;
        }
    } while(true);
}
}; // namespace utils
