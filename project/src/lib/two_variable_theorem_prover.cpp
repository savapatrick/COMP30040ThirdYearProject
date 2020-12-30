//
// Created by Patrick Sava on 12/29/2020.
//

#include "two_variable_theorem_prover.h"
#include "depth_ordered_theorem_prover.h"

using namespace std;

namespace utils {

bool TwoVariableTheoremProver::fullResolutionTwoVariableLiterals() {
    auto literalPredicate = [](shared_ptr<Literal>& first, shared_ptr<Literal>& second) -> bool {
        return (first->isNegated != second->isNegated) and (first->predicateName == second->predicateName) and
        (first->getAllVariables().size() == 2 and second->getAllVariables().size() == 2);
    };
    auto resolventPredicate = [](const std::shared_ptr<Literal>& resolvedLiteral, const std::vector<std::shared_ptr<Literal>> &resolvents) -> bool{
      return true;
    };
    do {
        int times = 0;
        if(factoringStep()) {
            outputStream << "derived empty clause!\n";
            return false;
        } else {
            times += 1;
        }
        if(resolutionStep(literalPredicate, resolventPredicate)) {
            outputStream << "derived empty clause!\n";
            return false;
        } else {
            times += 1;
        }
        if(times == 2 and hot.empty()) {
            outputStream << "reached saturation!\n";
            return true;
        }
    } while(true);
}


void TwoVariableTheoremProver::disposeTwoVariableClauses() {
    std::vector<std::shared_ptr<Clause>> newClauseForm;
    for(auto& clause : clauseForm->clauseForm) {
        if(clause->getHighestNumberOfVariablesPerLiteral() >= 2) {
            if(clause->getHighestNumberOfVariablesPerLiteral() > 2) {
                throw logic_error("we should not have more than two variables per clause at this point!");
            }
        } else {
            newClauseForm.push_back(clause);
        }
    }
    if(newClauseForm.size() != clauseForm->clauseForm.size()) {
        clauseForm->clauseForm = newClauseForm;
    }
}


bool TwoVariableTheoremProver::backtrackingClauseFormAndResolution(vector<std::shared_ptr<Literal>>& chosen) {
    if(chosen.size() == clauseForm->clauseForm.size()) {
        shared_ptr<ClauseForm> currentClauseForm(make_shared<ClauseForm>());
        for (auto &literal: chosen) {
            currentClauseForm->clauseForm.push_back({make_shared<Clause>(literal)});
        }
        DepthOrderedTheoremProver prover(currentClauseForm, false);
        // TODO: try to capture the output somehow
        return prover.run();
    }
    for (auto &elem: clauseForm->clauseForm[chosen.size()]->clause) {
        chosen.push_back(elem);
        if (backtrackingClauseFormAndResolution(chosen)) {
            return true;
        }
        chosen.pop_back();
    }
    return false;
}

bool TwoVariableTheoremProver::run() {
    outputStream << "we have the following clauses in our initial set!\n";
    outputStream << clauseForm->getStringWithIndex();
    if(!fullResolutionTwoVariableLiterals()) {
        outputData();
        return false;
    }
    disposeTwoVariableClauses();
    outputStream << "we have the following clauses after disposal:\n";
    outputStream << clauseForm->getStringWithIndex();
    vector<std::shared_ptr<Literal>> chosen;
    if (backtrackingClauseFormAndResolution(chosen)) {
        outputStream << "reached saturation!\n";
        outputData();
        return true;
    }
    outputStream << "derived empty clause!\n";
    outputData();
    return false;
}
}; // namespace utils
