//
// Created by Patrick Sava on 12/29/2020.
//

#include "two_variable_theorem_prover.h"
#include "depth_ordered_theorem_prover.h"
#include <iostream>

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
    outputData();
    if(resolutionStep<decltype(literalPredicate), decltype(resolventPredicate)>(literalPredicate, resolventPredicate)) {
        outputStream << "proved by deriving the empty clause!\n";
        outputData();
        return false;
    } else {
        outputStream << "refuted by reaching saturation!\n";
        outputData();
        return true;
    }
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

int currentTimestamp = 0;
bool TwoVariableTheoremProver::backtrackingClauseFormAndResolution(int currentChoice, shared_ptr<DepthOrderedTheoremProver>& prover) {
    ++currentTimestamp;                                                                          // TODO: comment
    cerr << to_string(currentTimestamp) + " depth " + to_string(currentChoice) << " a intrat\n"; // TODO: comment
    if(currentChoice == clauseForm->clauseForm.size()) {
        if(prover->run()) {
            outputStream << prover->getData();
            return true;
        }
        return false;
    } else {
        if(!prover->run()) {
            cerr << to_string(currentTimestamp) + " depth " + to_string(currentChoice) << "a crapat\n";
            return false;
        }
    }
    cerr.flush();
    map<set<string>, vector<shared_ptr<Literal>>> buckets;
    for(auto& elem : clauseForm->clauseForm[currentChoice]->clause) {
        auto variables = elem->getAllVariables();
        set<string> current(variables.begin(), variables.end());
        buckets[current].emplace_back(elem);
    }
    for(auto& elem : buckets) {
        prover->addNewClause(make_shared<Clause>(elem.second));
        if(backtrackingClauseFormAndResolution(currentChoice + 1, prover)) {
            return true;
        }
        prover->revert();
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
    outputData();
    shared_ptr<DepthOrderedTheoremProver> prover =
    make_shared<DepthOrderedTheoremProver>(make_shared<ClauseForm>(), "depth_ordered_theorem_prover.txt");
    if(backtrackingClauseFormAndResolution(0, prover)) {
        outputStream << "refuted by reaching saturation!\n";
        outputData();
        return true;
    }
    outputStream << "proved by deriving the empty clause!\n";
    outputData();
    return false;
}
}; // namespace utils
