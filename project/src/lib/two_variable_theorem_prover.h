//
// Created by Patrick Sava on 12/29/2020.
//

#ifndef PROJECT_TWO_VARIABLE_THEOREM_PROVER_H
#define PROJECT_TWO_VARIABLE_THEOREM_PROVER_H

#include "basic_theorem_prover.h"
namespace utils {
class TwoVariableTheoremProver : public BasicTheoremProver {
    private:
    bool withEquality;

    public:
    TwoVariableTheoremProver(std::shared_ptr<ClauseForm> _clauseForm,
    const std::string& _fileName = "theorem_prover.txt")
    : BasicTheoremProver(std::move(_clauseForm), _fileName), withEquality(false) {
        if (!clauseForm->isTwoVariableFragment()) {
            throw std::invalid_argument("The given clause form " + clauseForm->getString() + "is not a valid two variable fragment");
        }
    }
    bool fullResolutionTwoVariableLiterals();
    void disposeTwoVariableClauses();
    bool backtrackingClauseFormAndResolution(std::vector<std::shared_ptr<Clause>> &chosen);
    void run() override;
};
};

#endif // PROJECT_TWO_VARIABLE_THEOREM_PROVER_H
