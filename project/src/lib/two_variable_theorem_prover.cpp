//
// Created by Patrick Sava on 12/29/2020.
//

#include "two_variable_theorem_prover.h"
#include "depth_ordered_theorem_prover.h"

using namespace std;

namespace utils {

bool TwoVariableTheoremProver::fullResolutionTwoVariableLiterals() {
    auto literalPredicate = [](shared_ptr<Literal>& first, shared_ptr<Literal>& second) -> bool {
        auto p1 = first->getAllVariables();
        auto p2 = second->getAllVariables();
        return (first->isNegated != second->isNegated) and (first->predicateName == second->predicateName) and
        (first->getAllVariables().size() == 2 or second->getAllVariables().size() == 2);
    };
    auto resolventPredicate = [](const std::shared_ptr<Literal>& resolvedLiteral,
                                 const std::shared_ptr<Clause>& clause) -> bool { return true; };
    do {
        outputData();
        if(resolutionStep<decltype(literalPredicate), decltype(resolventPredicate)>(literalPredicate, resolventPredicate)) {
            outputStream << "proved by deriving the empty clause!\n";
            outputData();
            return false;
        } else {
            if(hot.empty()) {
                outputStream << "refuted by reaching saturation!\n";
                outputData();
                return true;
            }
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
        for(auto& literal : chosen) { currentClauseForm->clauseForm.push_back({ make_shared<Clause>(literal) }); }
        DepthOrderedTheoremProver prover(currentClauseForm);
        if(prover.run()) {
            outputStream << prover.getData();
            return true;
        }
        return false;
    }
    for(auto& elem : clauseForm->clauseForm[chosen.size()]->clause) {
        chosen.push_back(elem);
        if(backtrackingClauseFormAndResolution(chosen)) {
            return true;
        }
        chosen.pop_back();
    }
    return false;
}

bool TwoVariableTheoremProver::run() {
    outputStream << "[two variable theorem prover]\nwe have the following clauses in our initial set!\n";
    outputStream << clauseForm->getStringWithIndex();
    if(!fullResolutionTwoVariableLiterals()) {
        outputData();
        return false;
    }
    disposeTwoVariableClauses();
    outputStream << "[two variable theorem prover]\nwe have the following clauses after disposal:\n";
    outputStream << clauseForm->getStringWithIndex();
    vector<std::shared_ptr<Literal>> chosen;
    if(backtrackingClauseFormAndResolution(chosen)) {
        outputStream << "refuted by saturation!\n";
        outputData();
        return true;
    }
    outputStream << "proved by always deriving empty clause!\n";
    outputData();
    return false;
}
}; // namespace utils
