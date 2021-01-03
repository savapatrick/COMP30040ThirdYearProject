//
// Created by Patrick Sava on 11/25/2020.
//

#ifndef PROJECT_THEOREM_PROVER_H
#define PROJECT_THEOREM_PROVER_H

#include "clause_form.h"
#include "unordered_map"
#include <fstream>
#include <map>
#include <sstream>
#include <utility>
#include <vector>

namespace utils {
class TheoremProver {
    private:
    void clearFile();

    protected:
    std::shared_ptr<ClauseForm> clauseForm;
    std::stringstream outputStream;
    std::string filename;
    std::string allData;

    public:
    TheoremProver(const std::shared_ptr<ClauseForm>& _clauseForm, std::string _fileName = "theorem_prover.txt")
    : clauseForm(), outputStream(), filename(std::move(_fileName)) {
        clauseForm = std::make_shared<ClauseForm>(_clauseForm);
        clearFile();
    }
    std::string getData() const;
    void outputData();
    static bool isTautology(std::shared_ptr<Clause>& clause);
    virtual bool run() = 0;
};
} // namespace utils

#endif // PROJECT_THEOREM_PROVER_H
