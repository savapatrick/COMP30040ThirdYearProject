//
// Created by Patrick Sava on 11/25/2020.
//

#include "basic_theorem_prover.h"

namespace utils {

bool BasicTheoremProver::tryToUnifyTwoLiterals(std::shared_ptr<Clause>& clause) {
    auto clauseLiterals = clause->getAllLiterals();
    bool ok = false;
    for (auto &keyValue : clauseLiterals) {
        if (keyValue.second > 1) {
            ok = true;
            break;
        }
    }
    if (!ok) {
        return false;
    }
    // TODO: complete here
    return false;
}
bool BasicTheoremProver::isTautology(std::shared_ptr<Clause>& clause) {
    return false;
}
bool BasicTheoremProver::derivesEmptyClause(std::shared_ptr<Clause>& clause) {
    return false;
}
bool BasicTheoremProver::factoringStep() {
    std::vector<std::shared_ptr<Clause>> newClauseForm;
    for (auto &clause : clauseForm->clauseForm) {
        newClauseForm.push_back(clause);
        if (tryToUnifyTwoLiterals(clause)) {
            if (isTautology(clause)) {
                outputStream << "clause " + clause.getString() + " is a tautology, so it's dropped";
                newClauseForm.pop_back();
            }
            else if (derivesEmptyClause(clause)) {
                return true;
            }
        }
    }
    return false;
}
bool BasicTheoremProver::attemptToUnify(std::shared_ptr<Clause>& first, std::shared_ptr<Clause>& second) {
    return false;
}
bool BasicTheoremProver::resolutionStep() {
    return false;
}

void BasicTheoremProver::run() {
    do {
        if (factoringStep()) {
            outputStream << "derived empty clause!\n";
            break;
        }
        if (resolutionStep()) {
            outputStream << "derived empty clause!\n";
            break;
        }
    }while(true);
}
}; // namespace utils
