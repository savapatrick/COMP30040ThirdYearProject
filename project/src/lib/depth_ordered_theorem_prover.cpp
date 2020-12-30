//
// Created by Patrick Sava on 12/29/2020.
//

#include "depth_ordered_theorem_prover.h"

using namespace std;

namespace utils {

bool DepthOrderedTheoremProver::run() {
    outputStream << "we have the following clauses in our initial set!\n";
    outputStream << clauseForm->getStringWithIndex();
    auto literalPredicate = [](shared_ptr<Literal>& first, shared_ptr<Literal>& second) -> bool {
        return (first->isNegated != second->isNegated) and (first->predicateName == second->predicateName);
    };
    auto isAOrdering = [](const shared_ptr<Literal>& first, const shared_ptr<Literal>& second) -> bool {
        auto getDepthsFirst  = first->getDepths();
        auto getDepthsSecond = second->getDepths();
        if(getDepthsFirst.first >= getDepthsSecond.first) {
            return false;
        }
        auto variablesFirst  = first->getAllVariables();
        auto variablesSecond = second->getAllVariables();
        for(auto& variableFirst : variablesFirst) {
            if(variablesSecond.find(variableFirst) == variablesSecond.end()) {
                return false;
            }
        }
        for(auto& variableAndDepth : getDepthsFirst.second) {
            auto variable = variableAndDepth.first;
            auto depth    = variableAndDepth.second;
            if(depth >= getDepthsSecond.second[variable]) {
                return false;
            }
        }
        return true;
    };
    auto resolventPredicate = [&isAOrdering](const std::shared_ptr<Literal>& resolvedLiteral,
                              const std::vector<std::shared_ptr<Literal>>& resolvents) -> bool {
        for(auto& resolvent : resolvents) {
            if(isAOrdering(resolvedLiteral, resolvent)) {
                return false;
            }
        }
        return true;
    };
    do {
        outputData();
        int times = 0;
        if(factoringStep()) {
            outputStream << "derived empty clause!\n";
            outputData();
            return false;
        } else {
            times += 1;
        }
        if(resolutionStep<decltype(literalPredicate), decltype(resolventPredicate)>(literalPredicate, resolventPredicate)) {
            outputStream << "derived empty clause!\n";
            outputData();
            return false;
        } else {
            times += 1;
        }
        if(times == 2 and hot.empty()) {
            outputStream << "reached saturation!\n";
            outputData();
            return true;
        }
    } while(true);
}
}; // namespace utils