//
// Created by Patrick Sava on 11/25/2020.
//

#ifndef PROJECT_BASIC_THEOREM_PROVER_H
#define PROJECT_BASIC_THEOREM_PROVER_H

#include "theorem_prover.h"

#include <utility>

namespace utils {
class BasicTheoremProver : public TheoremProver {
    private:
    public:
    BasicTheoremProver(std::vector<ClauseForm::Clause> _clauseForm, std::ofstream& _outputStream)
    : TheoremProver(std::move(_clauseForm), _outputStream) {
    }
    virtual bool isSatisfiable() override;
};
} // namespace utils

#endif // PROJECT_BASIC_THEOREM_PROVER_H
