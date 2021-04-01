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
    outputStreamGuard.lock();
    outputStream << "removing duplicates from clause " + clause->getString() << "\n";
    outputStreamGuard.unlock();
    if(literals.size() != clause->clause.size()) {
        clause->clause.clear();
        clause->clause.reserve(literals.size());
        for(auto& keyValue : literals) { clause->clause.emplace_back(keyValue.second); }
        outputStreamGuard.lock();
        outputStream << "it then becomes " << clause->getString() << "\n";
        outputStreamGuard.unlock();
        return true;
    }
    return false;
}

void BasicTheoremProver::factoringStep() {
    bool changed = false;
    vector<shared_ptr<Clause>> toBeInserted;
    std::mutex isDeletedGuard;
    std::mutex setGuard;
    std::mutex changedGuard;
    std::mutex toBeInsertedGuard;
    if(firstSetOfSupportCheckpointIndex < previousState.size()) {
        int startPosition = previousState[firstSetOfSupportCheckpointIndex];
        std::vector<int> indexes;
        indexes.reserve((int)clauseForm->clauseForm.size());
        for(int index = startPosition; index < (int)clauseForm->clauseForm.size(); ++index) {
            indexes.push_back(index);
        }
        std::for_each(std::execution::par_unseq, std::begin(indexes), std::end(indexes), [&](auto&& index) {
            isDeletedGuard.lock();
            if(isDeleted.find(index) != isDeleted.end()) {
                isDeletedGuard.unlock();
                return;
            }
            isDeletedGuard.unlock();
            auto& clause      = clauseForm->clauseForm[index];
            auto previousHash = clause->getHash();
            if(removeDuplicates(clause)) {
                changedGuard.lock();
                changed = true;
                changedGuard.unlock();
                if(previousHash != clause->getHash()) {
                    setGuard.lock();
                    clausesSoFar.erase(clausesSoFar.find(previousHash));
                    clausesSoFar.insert(clause->getHash());
                    setGuard.unlock();
                    previousHash = clause->getHash();
                }
            }
            if(isTautology(clause)) {
                outputStreamGuard.lock();
                outputStream << "clause " + clause->getString() + " is a tautology, so it's dropped\n";
                outputStreamGuard.unlock();
                setGuard.lock();
                clausesSoFar.erase(clausesSoFar.find(previousHash));
                setGuard.unlock();
                changedGuard.lock();
                changed = true;
                changedGuard.unlock();
                isDeletedGuard.lock();
                isDeleted[index] = previousState.back();
                isDeletedGuard.unlock();
                return;
            }
            auto unificationResult = unification->tryToUnifyTwoLiterals(clause);
            for(auto& newClause : unificationResult) {
                if(!isTautology(newClause) and removeDuplicates(newClause)) {
                    setGuard.lock();
                    if(clausesSoFar.find(newClause->getHash()) == clausesSoFar.end()) {
                        setGuard.unlock();
                        toBeInsertedGuard.lock();
                        toBeInserted.push_back(newClause);
                        toBeInsertedGuard.unlock();
                        changedGuard.lock();
                        changed = true;
                        changedGuard.unlock();
                        continue;
                    }
                    setGuard.unlock();
                }
            }
        });
    }
    if(changed) {
        for(auto& elem : toBeInserted) {
            if(clausesSoFar.find(elem->getHash()) == clausesSoFar.end()) {
                clauseForm->clauseForm.push_back(elem);
                clausesSoFar.insert(elem->getHash());
            }
        }
    }
}

void BasicTheoremProver::subsumption() {
    vector<bool> toBeDeleted(clauseForm->clauseForm.size(), false);
    vector<int> byWhich(clauseForm->clauseForm.size(), 0);
    bool toBeModified = false;
    std::mutex toBeDeletedGuard;
    std::mutex toBeModifiedGuard;
    if(firstSetOfSupportCheckpointIndex < previousState.size()) {
        int startPosition = previousState[firstSetOfSupportCheckpointIndex];
        std::vector<int> indexes;
        indexes.reserve((int)clauseForm->clauseForm.size());
        for(int index = 0; index < (int)clauseForm->clauseForm.size(); ++index) { indexes.push_back(index); }
        std::for_each(std::execution::par_unseq, std::begin(indexes), std::end(indexes), [&](auto&& index) {
            if(isDeleted.find(index) != isDeleted.end()) {
                return;
            }
            toBeDeletedGuard.lock();
            if(toBeDeleted[index]) {
                toBeDeletedGuard.unlock();
                return;
            }
            toBeDeletedGuard.unlock();
            auto& clause    = clauseForm->clauseForm[index];
            auto hashSetOne = clause->getHashSet();
            for(int index2 = startPosition; index2 < (int)clauseForm->clauseForm.size(); ++index2) {
                if(isDeleted.find(index2) != isDeleted.end() or index2 <= index) {
                    continue;
                }
                toBeDeletedGuard.lock();
                if(toBeDeleted[index2]) {
                    toBeDeletedGuard.unlock();
                    continue;
                }
                toBeDeletedGuard.unlock();
                auto& clause2         = clauseForm->clauseForm[index2];
                const auto hashSetTwo = clause2->getHashSet();
                bool isSubsumed       = true;
                for(auto& x : hashSetOne) {
                    if(hashSetTwo.find(x) == hashSetTwo.end()) {
                        // that's a weak check; we could lose precious subsumptions
                        isSubsumed = false;
                        break;
                    }
                }
                if(!isSubsumed) {
                    isSubsumed = true;
                    for(auto& x : hashSetTwo) {
                        if(hashSetOne.find(x) == hashSetOne.end()) {
                            // that's a weak check; we could lose precious subsumptions
                            isSubsumed = false;
                            break;
                        }
                    }
                    if(isSubsumed) {
                        toBeDeletedGuard.lock();
                        toBeDeleted[index] = true;
                        byWhich[index]     = index2;
                        toBeDeletedGuard.unlock();
                        toBeModifiedGuard.lock();
                        toBeModified = true;
                        toBeModifiedGuard.unlock();
                    }
                    continue;
                }
                toBeDeletedGuard.lock();
                toBeDeleted[index2] = true;
                byWhich[index2]     = index;
                toBeDeletedGuard.unlock();
                toBeModifiedGuard.lock();
                toBeModified = true;
                toBeModifiedGuard.unlock();
            }
        });
    }
    if(!toBeModified) {
        return;
    }
    for(int index = 0; index < (int)clauseForm->clauseForm.size(); ++index) {
        auto& clause      = clauseForm->clauseForm[index];
        auto previousHash = clause->getHash();
        if(toBeDeleted[index]) {
            outputStream << "clause " + clause->getString() + " is being subsumed by " +
            clauseForm->clauseForm[byWhich[index]]->getString() + " so it's dropped\n";
            isDeleted[index] = previousState.back();
            continue;
        }
    }
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

int BasicTheoremProver::addNewClause(const std::shared_ptr<Clause>& newClause) {
    previousState.push_back(clauseForm->clauseForm.size());
    if(clausesSoFar.find(newClause->getHash()) == clausesSoFar.end()) {
        clausesSoFar.insert(newClause->getHash());
        clauseForm->clauseForm.push_back(newClause->createDeepCopy());
        clauseForm->makeVariableNamesUniquePerClause();
    }
    return previousState.back();
}
void BasicTheoremProver::revert(const int& checkpoint) {
    while(!previousState.empty() and previousState.back() >= checkpoint) { previousState.pop_back(); }
    while(clauseForm->clauseForm.size() > checkpoint) {
        auto whichClause = clauseForm->clauseForm.back();
        if(clausesSoFar.find(whichClause->getHash()) != clausesSoFar.end()) {
            clausesSoFar.erase(clausesSoFar.find(whichClause->getHash()));
        }
        clauseForm->clauseForm.pop_back();
    }
    vector<int> revive;
    for(auto& elem : isDeleted) {
        if(elem.second >= checkpoint) {
            revive.push_back(elem.first);
        }
    }
    while(!revive.empty()) {
        isDeleted.erase(isDeleted.find(revive.back()));
        revive.pop_back();
    }
    firstSetOfSupportCheckpointIndex = 0;
}
}; // namespace utils
