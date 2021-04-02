//
// Created by Patrick Sava on 12/29/2020.
//

#ifndef PROJECT_DEPTH_ORDERED_THEOREM_PROVER_H
#define PROJECT_DEPTH_ORDERED_THEOREM_PROVER_H

#include "basic_theorem_prover.h"

namespace utils {
class DepthOrderedTheoremProver : public BasicTheoremProver {
    private:
    public:
    DepthOrderedTheoremProver(const std::shared_ptr<ClauseForm>& _clauseForm,
    bool allowEquality           = false,
    const std::string& _fileName = "theorem_prover.txt")
    : BasicTheoremProver(_clauseForm, allowEquality, _fileName) {
    }
    bool run() override;
    bool boundedRun();
    bool unboundedRun();
};
}; // namespace utils

#endif // PROJECT_DEPTH_ORDERED_THEOREM_PROVER_H
