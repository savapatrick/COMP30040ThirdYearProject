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

void BasicTheoremProver::factoringStep() {
    bool changed = false;
    vector<shared_ptr<Clause>> newClauseForm;
    vector<shared_ptr<Clause>> toBeInserted;
    for(int index = 0; index < (int)clauseForm->clauseForm.size(); ++index) {
        auto& clause = clauseForm->clauseForm[index];
        changed |= removeDuplicates(clause);
        newClauseForm.push_back(clause);
        if(isTautology(clause)) {
            outputStream << "clause " + clause->getString() + " is a tautology, so it's dropped\n";
            newClauseForm.pop_back();
            changed = true;
            updateCache(index);
            continue;
        }
        auto unificationResult = unification->tryToUnifyTwoLiterals(clause);
        if(unificationResult.index()) {
            toBeInserted.push_back(std::get<1>(unificationResult));
            if(isTautology(toBeInserted.back()) or clausesSoFar.find(toBeInserted.back()->getString()) != clausesSoFar.end()) {
                toBeInserted.pop_back();
            } else {
                if(!removeDuplicates(toBeInserted.back())) {
                    toBeInserted.pop_back();
                } else {
                    changed = true;
                }
            }
        }
    }
    if(changed) {
        for(auto& elem : toBeInserted) { newClauseForm.push_back(elem); }
        clauseForm->clauseForm = newClauseForm;
    }
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
        if(resolutionStep<decltype(literalPredicate), decltype(resolventPredicate)>(literalPredicate, resolventPredicate)) {
            outputStream << "proved by deriving the empty clause!\n";
            outputData();
            return false;
        } else {
            if(hot.empty()) {
                outputStream << "refuted by reaching saturation!\n";
                outputData();
                return true;
            }
        }
    } while(true);
}

void BasicTheoremProver::updateCache(int deletedIndex) {
    if(hot.find(deletedIndex) != hot.end()) {
        hot.erase(hot.find(deletedIndex));
    }
    vector<int> toBeUpdated;
    for(auto& elem : hot) {
        if(elem.first > deletedIndex) {
            toBeUpdated.push_back(elem.first);
        }
    }
    vector<pair<int, long long>> toBeInserted;
    for(auto& elem : toBeUpdated) {
        auto value = hot[elem];
        hot.erase(hot.find(elem));
        toBeInserted.emplace_back(elem - 1, value);
    }
    for(auto& elem : toBeInserted) { hot[elem.first] = elem.second; }
    vector<pair<int, int>> toBeUpdatedSet;
    for(auto& elem : avoid) {
        if(elem.first >= deletedIndex or elem.second >= deletedIndex) {
            toBeUpdatedSet.emplace_back(elem);
        }
    }
    vector<pair<int, int>> toBeInsertedSet;
    for(auto elem : toBeUpdatedSet) {
        avoid.erase(avoid.find(elem));
        if(elem.first == deletedIndex or elem.second == deletedIndex) {
            continue;
        }
        if(elem.first > deletedIndex) {
            elem.first -= 1;
        }
        if(elem.second > deletedIndex) {
            elem.second -= 1;
        }
        toBeInsertedSet.emplace_back(elem);
    }
    for(auto& elem : toBeInsertedSet) { avoid.insert(elem); }
}
}; // namespace utils
