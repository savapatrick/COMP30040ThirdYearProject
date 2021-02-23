//
// Created by Patrick Sava on 12/29/2020.
//

#include "depth_ordered_theorem_prover.h"
#include <algorithm>
#include <functional>
#include <iostream>

using namespace std;

namespace utils {

bool DepthOrderedTheoremProver::run() {
    //    outputStream << "[depth ordered theorem prover]\nwe have the following clauses in our initial set!\n";
    //    outputStream << clauseForm->getStringWithIndex();
    auto literalPredicate = [](shared_ptr<Literal>& first, shared_ptr<Literal>& second) -> bool {
        return (first->isNegated != second->isNegated) and (first->predicateName == second->predicateName);
    };
    auto isAOrdering = [&](const shared_ptr<Literal>& first, const shared_ptr<Literal>& second) -> bool {
        //        outputStream << "[" << first->getString() << ", " << second->getString() << "]\n";
        auto getDepthsFirst  = first->getDepths();
        auto getDepthsSecond = second->getDepths();
        if(getDepthsFirst.second.empty() and getDepthsSecond.second.empty() and getDepthsFirst.first <= getDepthsSecond.first) {
            //            outputStream << "returns false\n";
            return false;
        }
        for(auto& variableAndDepth : getDepthsFirst.second) {
            auto variable = variableAndDepth.first;
            auto depth    = variableAndDepth.second;
            if(depth >= getDepthsSecond.second[variable]) {
                //                outputStream << "returns false\n";
                return false;
            }
        }
        for(auto& variableAndDepth : getDepthsSecond.second) {
            auto variable = variableAndDepth.first;
            auto depth    = variableAndDepth.second;
            if(depth <= getDepthsFirst.second[variable]) {
                //                outputStream << "returns false\n";
                return false;
            }
        }
        //        outputStream << "returns true\n";
        return true;
    };
    auto resolventPredicate = [&isAOrdering](const std::shared_ptr<Literal>& resolvedLiteral,
                              const std::shared_ptr<Clause>& clause) -> bool {
        auto resolvents = clause->getLiterals();
        for(auto& resolvent : resolvents) {
            if(isAOrdering(resolvent, resolvedLiteral)) {
                return false;
            }
        }
        return true;
    };
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
}; // namespace utils