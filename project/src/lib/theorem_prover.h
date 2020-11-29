//
// Created by Patrick Sava on 11/25/2020.
//

#ifndef PROJECT_THEOREM_PROVER_H
#define PROJECT_THEOREM_PROVER_H

#include "dual_hash_ascii.h"
#include "simplified_clause_form.h"
#include "unordered_map"
#include <map>
#include <utility>
#include <vector>

namespace utils {
class TheoremProver {
    protected:
    std::vector<SimplifiedClauseForm::SimplifiedClause> simplifiedClauseForm;
    std::ofstream& outputStream;
    bool hasLiteralAndNegatedLiteralInClause(const SimplifiedClauseForm::SimplifiedClause& clause);
    bool applyFactoringOnClauseIfPossible(SimplifiedClauseForm::SimplifiedClause* result,
    const SimplifiedClauseForm::SimplifiedClause& clause);
    [[nodiscard]] std::map<DualHashASCII::HashType, std::pair<bool, bool>> getLiteralsMap(
    const SimplifiedClauseForm::SimplifiedClause& first) const;
    [[nodiscard]] std::map<DualHashASCII::HashType, std::shared_ptr<SimplifiedLiteral>> getLiteralsInstances(
    const SimplifiedClauseForm::SimplifiedClause& first) const;

    public:
    TheoremProver(std::vector<SimplifiedClauseForm::SimplifiedClause> _clauseForm, std::ofstream& _outputStream)
    : simplifiedClauseForm(std::move(_clauseForm)), outputStream(_outputStream) {
    }
    bool checkForLiteralAndNegatedLiteralInClauses();
    bool applyFactoringOnClausesIfPossible();
    std::pair<bool, std::pair<SimplifiedClauseForm::SimplifiedClause, SimplifiedClauseForm::SimplifiedClause>>
    applyResolution(const SimplifiedClauseForm::SimplifiedClause& first, const SimplifiedClauseForm::SimplifiedClause& second);
    virtual bool isValid() = 0;
};
} // namespace utils

#endif // PROJECT_THEOREM_PROVER_H
