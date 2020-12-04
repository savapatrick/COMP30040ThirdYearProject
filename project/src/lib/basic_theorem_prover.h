//
// Created by Patrick Sava on 11/25/2020.
//

#ifndef PROJECT_BASIC_THEOREM_PROVER_H
#define PROJECT_BASIC_THEOREM_PROVER_H

#include "theorem_prover.h"
#include "clause.h"
#include <utility>

namespace utils {
class BasicTheoremProver : public TheoremProver {
    private:
    bool tryToUnifyTwoLiterals(std::shared_ptr<Clause>& clause); // and commit if possible
    bool isTautology(std::shared_ptr<Clause> &clause);
    bool derivesEmptyClause(std::shared_ptr<Clause> &clause);
    bool factoringStep();
    bool attemptToUnify(std::shared_ptr<Clause>& first, std::shared_ptr<Clause>& second);
    bool resolutionStep();

    public:
    BasicTheoremProver(std::shared_ptr<ClauseForm>  _clauseForm, const std::string& _fileName = "theorem_prover.txt") :
 TheoremProver(std::move(_clauseForm), _fileName){}
    void run() override;
};
} // namespace utils

#endif // PROJECT_BASIC_THEOREM_PROVER_H
