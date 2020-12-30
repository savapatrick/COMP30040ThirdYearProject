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
    DepthOrderedTheoremProver(std::shared_ptr<ClauseForm> _clauseForm, const std::string& _fileName = "theorem_prover.txt")
    : BasicTheoremProver(std::move(_clauseForm), _fileName) {
    }
    bool run() override;
};
}; // namespace utils

#endif // PROJECT_DEPTH_ORDERED_THEOREM_PROVER_H
