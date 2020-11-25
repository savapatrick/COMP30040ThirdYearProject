//
// Created by Patrick Sava on 11/25/2020.
//

#ifndef PROJECT_THEOREM_PROVER_H
#define PROJECT_THEOREM_PROVER_H

#include "clause_form.h"
#include <utility>
#include <vector>

namespace utils {
class TheoremProver {
    protected:
    std::vector<ClauseForm::Clause> clauseForm;
    std::ofstream& outputStream;
    bool hasLiteralAndNegatedLiteralInClause(const ClauseForm::Clause& clause);

    public:
    TheoremProver(std::vector<ClauseForm::Clause> _clauseForm, std::ofstream& _outputStream)
    : clauseForm(std::move(_clauseForm)), outputStream(_outputStream) {
    }
    bool checkForLiteralAndNegatedLiteralInClauses();
    virtual bool isSatisfiable() = 0;
};
} // namespace utils

#endif // PROJECT_THEOREM_PROVER_H
