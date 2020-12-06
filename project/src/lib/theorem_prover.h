//
// Created by Patrick Sava on 11/25/2020.
//

#ifndef PROJECT_THEOREM_PROVER_H
#define PROJECT_THEOREM_PROVER_H

#include "clause_form.h"
#include "unordered_map"
#include <fstream>
#include <map>
#include <utility>
#include <vector>

namespace utils {
class TheoremProver {
    private:
    protected:
    std::shared_ptr<ClauseForm> clauseForm;
    std::ofstream outputStream;

    public:
    TheoremProver(std::shared_ptr<ClauseForm> _clauseForm, const std::string& _fileName = "theorem_prover.txt")
    : clauseForm(std::move(_clauseForm)), outputStream(_fileName.c_str(), std::ios::out) {
    }
    virtual void run() = 0;
};
} // namespace utils

#endif // PROJECT_THEOREM_PROVER_H
