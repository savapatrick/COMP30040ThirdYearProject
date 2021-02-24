//
// Created by Patrick Sava on 11/25/2020.
//

#ifndef PROJECT_BASIC_THEOREM_PROVER_H
#define PROJECT_BASIC_THEOREM_PROVER_H

#include "clause.h"
#include "clause_form.h"
#include "theorem_prover.h"
#include "unification.h"

#include <utility>

namespace utils {
class BasicTheoremProver : public TheoremProver {
    protected:
    std::shared_ptr<Unification> unification;
    std::set<std::pair<int, int>> avoid;
    std::unordered_map<std::string, std::shared_ptr<Clause>> clauses;
    std::unordered_set<std::string> clausesSoFar;
    std::vector<int> previousState;

    void updateCache(int deletedIndex);
    bool removeDuplicates(std::shared_ptr<Clause>& clause);
    void factoringStep();
    template <typename LiteralPredicate, typename ResolventPredicate>
    bool resolutionStep(LiteralPredicate literalPredicate, ResolventPredicate resolventPredicate);

    public:
    BasicTheoremProver(std::shared_ptr<ClauseForm> _clauseForm, const std::string& _fileName = "theorem_prover.txt")
    : TheoremProver(_clauseForm, _fileName), unification(std::make_shared<Unification>(outputStream)) {
        avoid.clear();
        clauses.clear();
        clausesSoFar.clear();
        previousState.clear();
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
    void addNewClause(const std::shared_ptr<Clause>& newClause);
    void revert();
    bool run() override;
};

template <typename LiteralPredicate, typename ResolventPredicate>
bool BasicTheoremProver::resolutionStep(LiteralPredicate literalPredicate, ResolventPredicate resolventPredicate) {
    do {
//        outputStream << "[DEBUG] the size of clauseForm is " + std::to_string(clauseForm->clauseForm.size()) << '\n';
        outputData();
        clauses.clear();
        factoringStep();
        for(int index = 0; index < (int)clauseForm->clauseForm.size(); ++index) {
            for(int index2 = index; index2 < (int)clauseForm->clauseForm.size(); ++index2) {
                if(avoid.find({ index, index2 }) != avoid.end()) {
                    continue;
                }
                auto result = unification->attemptToUnify<decltype(literalPredicate), decltype(resolventPredicate)>(
                clauseForm->clauseForm[index], clauseForm->clauseForm[index2], literalPredicate, resolventPredicate);
//                outputStream << "[DEBUG] " << index << " " << index2 << " were processed\n";
//                outputStream.flush();
                avoid.insert({ index, index2 });
                if(!result.empty()) {
                    for(auto& currentClause : result) {
                        removeDuplicates(currentClause);
                        if(isTautology(currentClause)) {
                            continue;
                        }
                        auto clauseHash = currentClause->getHash();
                        if(clauses.find(clauseHash) != clauses.end()) {
                            continue;
                        }
                        if(clausesSoFar.find(clauseHash) != clausesSoFar.end()) {
                            continue;
                        }
                        outputStream << "[ADD] we managed to unify " << clauseForm->clauseForm[index]->getString()
                                     << " with " << clauseForm->clauseForm[index2]->getString()
                                     << "and it results a new clause " << currentClause->getString() << '\n';
                        clauses[clauseHash] = currentClause;
                        clausesSoFar.insert(clauseHash);
                        if(currentClause->clause.empty()) {
                            // we derived the empty clause
                            for(auto& keyValue : clauses) { clauseForm->clauseForm.push_back(keyValue.second); }
                            outputData();
                            return true;
                        }
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

} // namespace utils

#endif // PROJECT_BASIC_THEOREM_PROVER_H
