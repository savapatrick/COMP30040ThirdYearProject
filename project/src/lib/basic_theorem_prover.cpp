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
        auto& clause      = clauseForm->clauseForm[index];
        auto previousHash = clause->getHash();
        if(removeDuplicates(clause)) {
            changed = true;
            if(previousHash != clause->getHash()) {
                clausesSoFar.erase(clausesSoFar.find(previousHash));
                previousHash = clause->getHash();
                clausesSoFar.insert(previousHash);
            }
        }
        newClauseForm.push_back(clause);
        if(isTautology(clause)) {
            outputStream << "clause " + clause->getString() + " is a tautology, so it's dropped\n";
            newClauseForm.pop_back();
            clausesSoFar.erase(clausesSoFar.find(previousHash));
            changed = true;
            updateCache(index);
            continue;
        }
        auto unificationResult = unification->tryToUnifyTwoLiterals(clause);
        if(!unificationResult.empty()) {
            for(auto& newClause : unificationResult) {
                // TODO: consider removing isTautology and removeDuplicates
                if(!isTautology(newClause) and removeDuplicates(newClause) and
                clausesSoFar.find(newClause->getHash()) == clausesSoFar.end()) {
                    toBeInserted.push_back(newClause);
                    changed = true;
                }
            }
        }
    }
    if(changed) {
        for(auto& elem : toBeInserted) {
            if(clausesSoFar.find(elem->getHash()) == clausesSoFar.end()) {
                newClauseForm.push_back(elem);
                clausesSoFar.insert(elem->getHash());
            }
        }
        clauseForm->clauseForm = newClauseForm;
    }
}

void BasicTheoremProver::subsumption() {
    vector <bool> toBeDeleted(clauseForm->clauseForm.size(), false);
    bool toBeModified = false;
    for(int index = 0; index < (int)clauseForm->clauseForm.size(); ++index) {
        auto& clause      = clauseForm->clauseForm[index];
        auto hashSetOne = clause->getHashSet();
        for (int index2 = index + 1; index2 < (int)clauseForm->clauseForm.size(); ++ index2) {
            if (toBeDeleted[index2]) {
                continue;
            }
            auto& clause2      = clauseForm->clauseForm[index2];
            auto hashSetTwo = clause2->getHashSet();
            bool isSubsumed = true;
            for (auto &x : hashSetOne) {
                if (hashSetTwo.find(x) == hashSetTwo.end()) {
                    isSubsumed = false;
                    break;
                }
            }
            if (!isSubsumed) {continue;}
            toBeDeleted[index2] = true;
            toBeModified = true;
        }
    }
    if (!toBeModified) {
        return;
    }
    vector<shared_ptr<Clause>> newClauseForm;
    for (int index = 0; index < (int)clauseForm->clauseForm.size(); ++ index) {
        auto& clause      = clauseForm->clauseForm[index];
        auto previousHash = clause->getHash();
        newClauseForm.push_back(clause);
        if (toBeDeleted[index]) {
            outputStream << "clause " + clause->getString() + " is being subsumed, so it's dropped\n";
            newClauseForm.pop_back();
            clausesSoFar.erase(clausesSoFar.find(previousHash));
            updateCache(index);
            continue;
        }
    }
    clauseForm->clauseForm = newClauseForm;
}

bool BasicTheoremProver::run() {
    upperLimit = std::numeric_limits<long long>::max();
    outputStream << "[basic theorem prover]\nwe have the following clauses in our initial set!\n";
    outputStream << clauseForm->getStringWithIndex();
    auto literalPredicate = [](shared_ptr<Literal>& first, shared_ptr<Literal>& second) -> bool {
        return (first->isNegated != second->isNegated) and (first->predicateName == second->predicateName);
    };
    auto resolventPredicate = [](const std::shared_ptr<Literal>& resolvedLiteral,
                              const std::shared_ptr<Clause>& clause) -> bool { return true; };
    outputData();
    if(resolutionStep<decltype(literalPredicate), decltype(resolventPredicate)>(literalPredicate, resolventPredicate)) {
        outputStream << "proved by deriving the empty clause!\n";
        outputData();
        return false;
    } else {
        outputStream << "refuted by reaching saturation!\n";
        outputData();
        return true;
    }
}

void BasicTheoremProver::updateCache(int deletedIndex) {
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
void BasicTheoremProver::addNewClause(const std::shared_ptr<Clause>& newClause) {
    previousState.push_back(clauseForm->clauseForm.size());
    if(clausesSoFar.find(newClause->getHash()) == clausesSoFar.end()) {
        clausesSoFar.insert(newClause->getHash());
        clauseForm->clauseForm.push_back(newClause->createDeepCopy());
        clauseForm->makeVariableNamesUniquePerClause();
    }
}
void BasicTheoremProver::revert() {
    if(previousState.empty()) {
        throw logic_error("[BasicTheoremProver] Cannot revert when there are no checkpoints!");
    }
    int previousLabel = previousState.back();
    previousState.pop_back();
    while(clauseForm->clauseForm.size() > previousLabel) {
        auto whichClause = clauseForm->clauseForm.back();
        clausesSoFar.erase(clausesSoFar.find(whichClause->getHash()));
        clauseForm->clauseForm.pop_back();
    }
    vector<pair<int, int>> toBeDeleted;
    for(auto& elem : avoid) {
        if(elem.first >= previousLabel or elem.second >= previousLabel) {
            toBeDeleted.push_back(elem);
        }
    }
    while(!toBeDeleted.empty()) {
        avoid.erase(avoid.find(toBeDeleted.back()));
        toBeDeleted.pop_back();
    }
}
}; // namespace utils
