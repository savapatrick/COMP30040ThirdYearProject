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
    outputStream.flush();
    if(literals.size() != clause->clause.size()) {
        clause->clause.clear();
        clause->clause.reserve(literals.size());
        for(auto& keyValue : literals) { clause->clause.emplace_back(keyValue.second); }
        outputStream << "it then becomes " << clause->getString() << "\n";
        outputStream.flush();
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
        if(unification->tryToUnifyTwoLiterals(clause)) {
            if(isTautology(clause)) {
                outputStream << "clause " + clause->getString() + " is a tautology, so it's dropped\n";
                outputStream.flush();
                newClauseForm.pop_back();
                changed = true;
                // TODO: instead of clear,
                // strongly prefer over manually tweak the indices
                avoid.clear();
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

template <class LiteralPredicate> bool BasicTheoremProver::resolutionStep(LiteralPredicate predicate) {
    do {
        clauses.clear();
        timestamp += 1;
        for(int index = 0; index < (int)clauseForm->clauseForm.size(); ++index) {
            for(int index2 = index + 1; index2 < (int)clauseForm->clauseForm.size(); ++index2) {
                if(avoid.find({ index, index2 }) != avoid.end() or (hot.find(index) != hot.end() and hot[index] < timestamp) or
                (hot.find(index2) != hot.end() and hot[index2] < timestamp)) {
                    continue;
                }
                if(hot.find(index) != hot.end()) {
                    hot.erase(hot.find(index));
                }
                if(hot.find(index2) != hot.end()) {
                    hot.erase(hot.find(index2));
                }
                auto result = unification->attemptToUnify(clauseForm->clauseForm[index], clauseForm->clauseForm[index2], predicate);
                // outputStream << "[DEBUG] " << index << " " << index2 << " were processed\n";
                // outputStream.flush();
                avoid.insert({ index, index2 });
                if(result.first) {
                    //  outputStream << "[DEBUG] we managed to unify " << clauseForm->clauseForm[index]->getString() <<
                    //  " with " << clauseForm->clauseForm[index2]->getString() << "and it results a new clause " <<
                    //  result.second->getString() << '\n';
                    //  outputStream.flush();
                    auto clauseHash = result.second->getString();
                    if(clauses.find(clauseHash) != clauses.end()) {
                        continue;
                    }
                    if(clausesSoFar.find(clauseHash) != clausesSoFar.end()) {
                        continue;
                    }
                    clauses[clauseHash] = result.second;
                    clausesSoFar.insert(clauseHash);
                    hot[index] = timestamp + static_cast<long long>(clauseForm->clauseForm.size()) + 1; // then pick it again
                    hot[index2] = timestamp + static_cast<long long>(clauseForm->clauseForm.size()) + 1; // then pick it again
                    if(result.second->clause.empty()) {
                        // we derived the empty clause
                        return true;
                    }
                }
            }
        }
        if(!clauses.empty()) {
            for(auto& keyValue : clauses) { clauseForm->clauseForm.push_back(keyValue.second); }
            clauseForm->makeVariableNamesUniquePerClause();
        } else {
            return false;
        }
    } while(true);
}

void BasicTheoremProver::run() {
    outputStream << "we have the following clauses in our initial set!\n";
    outputStream << clauseForm->getStringWithIndex();
    outputStream.flush();
    auto predicate = [](shared_ptr<Literal>& first, shared_ptr<Literal>& second) -> bool {
        return (first->isNegated != second->isNegated) and (first->predicateName == second->predicateName);
    };
    do {
        int times = 0;
        if(factoringStep()) {
            outputStream << "derived empty clause!\n";
            outputStream.flush();
            break;
        } else {
            times += 1;
        }
        if(resolutionStep(predicate)) {
            outputStream << "derived empty clause!\n";
            outputStream.flush();
            break;
        } else {
            times += 1;
        }
        if(times == 2 and hot.empty()) {
            outputStream << "reached saturation!\n";
            outputStream.flush();
            break;
        }
    } while(true);
}
}; // namespace utils
