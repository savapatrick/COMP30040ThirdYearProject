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
    template <class LiteralPredicate> bool resolutionStep(LiteralPredicate predicate);

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
    void run() override;
};

} // namespace utils

#endif // PROJECT_BASIC_THEOREM_PROVER_H
