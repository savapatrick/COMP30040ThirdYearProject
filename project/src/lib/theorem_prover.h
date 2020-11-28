//
// Created by Patrick Sava on 11/25/2020.
//

#ifndef PROJECT_THEOREM_PROVER_H
#define PROJECT_THEOREM_PROVER_H

#include "dual_hash_ascii.h"
#include "unordered_map"
#include "clause_form.h"
#include <map>
#include <utility>
#include <vector>

namespace utils {
class TheoremProver {
    protected:
    std::vector<ClauseForm::Clause> clauseForm;
    std::ofstream& outputStream;
    bool hasLiteralAndNegatedLiteralInClause(const ClauseForm::Clause& clause);
    bool applyFactoringOnClauseIfPossible(ClauseForm::Clause* result, const ClauseForm::Clause& clause);
    [[nodiscard]] std::map<DualHashASCII::HashType, std::pair<bool, bool>> getLiteralsMap(const ClauseForm::Clause& first) const;
    [[nodiscard]] std::map<DualHashASCII::HashType, std::shared_ptr<Literal>> getLiteralsInstances(
    const ClauseForm::Clause& first) const;

    public:
    TheoremProver(std::vector<ClauseForm::Clause> _clauseForm, std::ofstream& _outputStream)
    : clauseForm(std::move(_clauseForm)), outputStream(_outputStream) {
    }
    bool checkForLiteralAndNegatedLiteralInClauses();
    bool applyFactoringOnClausesIfPossible();
    std::pair<bool, std::pair<ClauseForm::Clause, ClauseForm::Clause>>
    applyResolution(const ClauseForm::Clause& first, const ClauseForm::Clause& second);
    virtual bool isValid() = 0;
};
} // namespace utils

#endif // PROJECT_THEOREM_PROVER_H
