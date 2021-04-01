//
// Created by Patrick Sava on 11/25/2020.
//

#ifndef PROJECT_BASIC_THEOREM_PROVER_H
#define PROJECT_BASIC_THEOREM_PROVER_H

#include "clause.h"
#include "clause_form.h"
#include "theorem_prover.h"
#include "unification.h"

#include <algorithm>
#include <execution>
#include <iostream>
#include <mutex>
#include <utility>

namespace utils {
class BasicTheoremProver : public TheoremProver {
    protected:
    std::mutex outputStreamGuard;
    std::shared_ptr<Unification> unification;
    std::unordered_map<std::string, std::shared_ptr<Clause>> clauses;
    std::unordered_set<std::string> clausesSoFar;
    std::vector<std::pair<int, int>> previousState;
    std::unordered_map<int, int> isDeleted;
    int firstSetOfSupportCheckpointIndex;
    long long upperLimit;

    bool removeDuplicates(std::shared_ptr<Clause>& clause);
    void factoringStep();
    void subsumption();
    template <typename LiteralPredicate, typename ResolventPredicate>
    bool resolutionStep(LiteralPredicate literalPredicate, ResolventPredicate resolventPredicate);

    public:
    BasicTheoremProver(const std::shared_ptr<ClauseForm>& _clauseForm, bool allowEquality = false, const std::string& _fileName = "theorem_prover.txt")
    : TheoremProver(_clauseForm, allowEquality, _fileName), outputStreamGuard() {
        unification = std::make_shared<Unification>(outputStreamGuard, outputStream);
        clauses.clear();
        clausesSoFar.clear();
        previousState.clear();
        isDeleted.clear();
        firstSetOfSupportCheckpointIndex = 0;
        upperLimit                       = std::numeric_limits<long long>::max(); // something HUGE
        previousState.emplace_back(0, firstSetOfSupportCheckpointIndex);
        std::vector<std::shared_ptr<Clause>> newClauseForm;
        for(auto& elem : clauseForm->clauseForm) {
            removeDuplicates(elem);
            if(isTautology(elem)) {
                continue;
            }
            if(clausesSoFar.find(elem->getHash()) == clausesSoFar.end()) {
                clausesSoFar.insert(elem->getHash());
                newClauseForm.push_back(elem);
            }
        }
        if(clauseForm->clauseForm.size() != newClauseForm.size()) {
            clauseForm->clauseForm = newClauseForm;
        }
        clauseForm->makeVariableNamesUniquePerClause();
    }
    int addNewClause(const std::shared_ptr<Clause>& newClause);
    void revert(const int& checkpoint);
    bool run() override;
};

template <typename LiteralPredicate, typename ResolventPredicate>
bool BasicTheoremProver::resolutionStep(LiteralPredicate literalPredicate, ResolventPredicate resolventPredicate) {
    do {
        std::cerr << "[SIZE] clauseForm.size() is " + std::to_string(clauseForm->clauseForm.size()) << '\n';
        std::cerr.flush();
        outputData();
        clauses.clear();
        std::cerr << "enters inside factoring!\n";
        std::cerr.flush();
        factoringStep();
        std::cerr << "it's outside factoring!\n";
        std::cerr.flush();
        std::cerr << "enters inside subsumption!\n";
        std::cerr.flush();
        subsumption();
        std::cerr << "it's outside subsumption!\n";
        std::cerr.flush();
        std::mutex insertGuard;
        std::vector<int> indexes;
        indexes.reserve((int)clauseForm->clauseForm.size());
        for(int index = 0; index < (int)clauseForm->clauseForm.size(); ++index) { indexes.push_back(index); }
        if(firstSetOfSupportCheckpointIndex < previousState.size()) {
            std::cerr << "enters inside multithreading!\n";
            std::cerr.flush();
            int startPosition = previousState[firstSetOfSupportCheckpointIndex].first;
            std::for_each(std::execution::par_unseq, std::begin(indexes), std::end(indexes), [&](auto&& index) {
                if(isDeleted.find(index) == isDeleted.end()) {
                    for(int index2 = startPosition; index2 < (int)clauseForm->clauseForm.size(); ++index2) {
                        if(isDeleted.find(index2) != isDeleted.end()) {
                            continue;
                        }
                        auto result = unification->attemptToUnify<decltype(literalPredicate), decltype(resolventPredicate)>(
                        clauseForm->clauseForm[index], clauseForm->clauseForm[index2], literalPredicate, resolventPredicate);
                        if(!result.empty()) {
                            for(auto& currentClause : result) {
                                if(isTautology(currentClause)) {
                                    continue;
                                }
                                removeDuplicates(currentClause);
                                auto clauseHash = currentClause->getHash();
                                insertGuard.lock();
                                if(clauses.find(clauseHash) != clauses.end() or
                                clausesSoFar.find(clauseHash) != clausesSoFar.end()) {
                                    insertGuard.unlock();
                                    continue;
                                }
                                insertGuard.unlock();
                                outputStreamGuard.lock();
                                outputStream << "[ADD] we managed to unify " << clauseForm->clauseForm[index]->getString()
                                             << " with " << clauseForm->clauseForm[index2]->getString()
                                             << "and it results a new clause " << currentClause->getString() << '\n';
                                outputStreamGuard.unlock();
                                insertGuard.lock();
                                clauses[clauseHash] = currentClause;
                                clausesSoFar.insert(clauseHash);
                                insertGuard.unlock();
                            }
                        }
                    }
                }
            });
            std::cerr << "it's outside multithreading!\n";
            std::cerr.flush();
        }
        firstSetOfSupportCheckpointIndex++;
        if(!clauses.empty()) {
            previousState.emplace_back(clauseForm->clauseForm.size(), firstSetOfSupportCheckpointIndex);
            bool derivedEmpty = false;
            for(auto& keyValue : clauses) {
                auto& currentClause = keyValue.second;
                clauseForm->clauseForm.push_back(currentClause);
                if(currentClause->clause.empty()) {
                    derivedEmpty = true;
                }
            }
            if(derivedEmpty) {
                outputData();
                return true;
            }
            clauseForm->makeVariableNamesUniquePerClause();
        } else {
            return false;
        }
    } while(upperLimit-- > 0);
    return false;
}

} // namespace utils

#endif // PROJECT_BASIC_THEOREM_PROVER_H
