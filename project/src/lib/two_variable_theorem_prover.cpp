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
        if(first->isNegated == second->isNegated or first->predicateName != second->predicateName) {
            return false;
        }
        auto p1 = first->getAllVariables();
        auto p2 = second->getAllVariables();
        return (first->getAllVariables().size() == 2 or second->getAllVariables().size() == 2) and
        !first->isEquality and !second->isEquality;
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
        if(clause->getHighestNumberOfVariablesPerLiteralExcludingEquality() >= 2) {
            if(clause->getHighestNumberOfVariablesPerLiteralExcludingEquality() > 2) {
                throw logic_error("we should not have more than two variables per clause at this point!");
            }
        } else {
            newClauseForm.push_back(clause);
        }
    }
    if(newClauseForm.size() != clauseForm->clauseForm.size()) {
        clauseForm->clauseForm = newClauseForm;
        // todo: is it really correct?
        for(auto& clause : newClauseForm) {
            auto variables = clause->getAllVariables();
            if(variables.size() > 1) {
                auto fixedVariable = *variables.begin();
                variables.erase(variables.begin());
                for(auto& variable : variables) { clause->applySubstitution({ variable, fixedVariable }); }
            }
        }
    }
}

bool TwoVariableTheoremProver::backtrackingClauseFormAndResolution(int currentChoice, shared_ptr<DepthOrderedTheoremProver>& prover) {
    if(currentChoice == clauseForm->clauseForm.size()) {
        if(prover->unboundedRun()) {
            outputStream << prover->getData();
            return true;
        }
        return false;
    } else {
        if(!prover->boundedRun()) {
            return false;
        }
    }
    map<set<string>, vector<shared_ptr<Literal>>> buckets;
    for(auto& elem : clauseForm->clauseForm[currentChoice]->clause) {
        auto variables = elem->getAllVariables();
        set<string> current(variables.begin(), variables.end());
        buckets[current].emplace_back(elem);
    }
    for(auto& elem : buckets) {
        const int checkpoint = prover->addNewClause(make_shared<Clause>(elem.second));
        if(backtrackingClauseFormAndResolution(currentChoice + 1, prover)) {
            return true;
        }
        prover->revert(checkpoint);
    }
    return false;
}

bool TwoVariableTheoremProver::run() {
    upperLimit = std::numeric_limits<long long>::max();
    outputStream << "[two variable theorem prover]\nwe have the following clauses in our initial set!\n";
    outputStream << clauseForm->getStringWithIndex();
    cerr << "[SIZE] : " << clauseForm->clauseForm.size() << '\n';
    cerr << "entering in full resolution on two variable literals!";
    if(!fullResolutionTwoVariableLiterals()) {
        outputData();
        return false;
    }
    cerr << "exiting in full resolution on two variable literals!";
    cerr.flush();
    cerr << "[SIZE before disposal] : " << clauseForm->clauseForm.size() << '\n';
    outputStream << "[two variable theorem prover]\nwe have the following clauses before disposal:\n";
    outputStream << clauseForm->getStringWithIndex();
    outputData();
    disposeTwoVariableClauses();
    cerr << "[SIZE after disposal] : " << clauseForm->clauseForm.size() << '\n';
    outputStream << "[two variable theorem prover]\nwe have the following clauses after disposal:\n";
    outputStream << clauseForm->getStringWithIndex();
    outputData();
    if(withEquality and clauseForm->containsEquality()) {
        if(!clauseForm->makeTwoVariableFragment()) {
            throw logic_error("The intermediate representation of the given set of formulas in an invalid "
                              "two variable fragment: " +
            clauseForm->getString());
        }
        clauseForm->resolveEquality();
    }
    cerr << "[SIZE after equality] : " << clauseForm->clauseForm.size() << '\n';
    shared_ptr<DepthOrderedTheoremProver> prover =
    make_shared<DepthOrderedTheoremProver>(make_shared<ClauseForm>(), true, "depth_ordered_theorem_prover.txt");
    cerr << "entering in backtracking!\n";
    cerr.flush();
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
