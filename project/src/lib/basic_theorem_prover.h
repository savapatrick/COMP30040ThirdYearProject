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
    std::unordered_map<int, long long> hot;
    std::unordered_map<std::string, std::shared_ptr<Clause>> clauses;
    std::unordered_set<std::string> clausesSoFar;
    long long timestamp;
    bool removeDuplicates(std::shared_ptr<Clause>& clause);
    bool factoringStep();
    template <typename LiteralPredicate, typename ResolventPredicate>
    bool resolutionStep(LiteralPredicate literalPredicate, ResolventPredicate resolventPredicate);

    public:
    BasicTheoremProver(std::shared_ptr<ClauseForm> _clauseForm, const std::string& _fileName = "theorem_prover.txt")
    : TheoremProver(std::move(_clauseForm), _fileName), unification(std::make_shared<Unification>(outputStream)) {
        avoid.clear();
        hot.clear();
        clauses.clear();
        clausesSoFar.clear();
        timestamp = 0;
        for(auto& elem : clauseForm->clauseForm) { clausesSoFar.insert(elem->getString()); }
    }
    bool run() override;
};

template <typename LiteralPredicate, typename ResolventPredicate>
bool BasicTheoremProver::resolutionStep(LiteralPredicate literalPredicate, ResolventPredicate resolventPredicate) {
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
                auto result = unification->attemptToUnify<decltype(literalPredicate), decltype(resolventPredicate)>(
                clauseForm->clauseForm[index], clauseForm->clauseForm[index2], literalPredicate, resolventPredicate);
                // outputStream << "[DEBUG] " << index << " " << index2 << " were processed\n";
                avoid.insert({ index, index2 });
                if(result.first) {
                    //  outputStream << "[DEBUG] we managed to unify " << clauseForm->clauseForm[index]->getString() <<
                    //  " with " << clauseForm->clauseForm[index2]->getString() << "and it results a new clause " <<
                    //  result.second->getString() << '\n';
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

} // namespace utils

#endif // PROJECT_BASIC_THEOREM_PROVER_H
