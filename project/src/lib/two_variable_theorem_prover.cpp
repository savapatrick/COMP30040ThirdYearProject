//
// Created by Patrick Sava on 12/29/2020.
//

#include "two_variable_theorem_prover.h"
#include "depth_ordered_theorem_prover.h"

using namespace std;

namespace utils {

bool TwoVariableTheoremProver::fullResolutionTwoVariableLiterals() {
    auto predicate = [](shared_ptr<Literal>& first, shared_ptr<Literal>& second) -> bool {
      return (first->isNegated != second->isNegated) and (first->predicateName == second->predicateName)
        and (first->getAllVariables().size() == 2 and second->getAllVariables().size() == 2);
    };
    do {
        int times = 0;
        if(factoringStep()) {
            outputStream << "derived empty clause!\n";
            outputStream.flush();
            return true;
        }
        else {
            times += 1;
        }
        if(resolutionStep(predicate)) {
            outputStream << "derived empty clause!\n";
            outputStream.flush();
            return true;
        }
        else {
            times += 1;
        }
        if (times == 2 and hot.empty()) {
            outputStream << "reached saturation!\n";
            outputStream.flush();
            return false;
        }
    } while(true);
}


void TwoVariableTheoremProver::disposeTwoVariableClauses() {
    std::vector<std::shared_ptr<Clause>> newClauseForm;
    for (auto& clause : clauseForm->clauseForm) {
        if (clause->getHighestNumberOfVariablesPerLiteral() >= 2) {
            if (clause->getHighestNumberOfVariablesPerLiteral() > 2) {
                throw logic_error("we should not have more than two variables per clause at this point!");
            }
        }
        else {
            newClauseForm.push_back(clause);
        }
    }
    if (newClauseForm.size() != clauseForm->clauseForm.size()) {
        clauseForm->clauseForm = newClauseForm;
    }
}


bool TwoVariableTheoremProver::backtrackingClauseFormAndResolution(vector<std::shared_ptr<Clause>>& chosen) {
    if(chosen.size() == clauseForm->clauseForm.size()) {
        shared_ptr<ClauseForm> currentClauseForm(make_shared<ClauseForm>());
        currentClauseForm->clauseForm = chosen;
        DepthOrderedTheoremProver prover(currentClauseForm);
        // TODO: continue from here
    }
}

void TwoVariableTheoremProver::run() {
    outputStream << "we have the following clauses in our initial set!\n";
    outputStream << clauseForm->getStringWithIndex();
    outputStream.flush();
    if (fullResolutionTwoVariableLiterals()) {
        return;
    }
    disposeTwoVariableClauses();
    outputStream << "we have the following clauses after disposal:\n";
    outputStream << clauseForm->getStringWithIndex();
    outputStream.flush();

}
};
