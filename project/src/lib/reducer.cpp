//
// Created by Patrick Sava on 11/4/2020.
//

#include "reducer.h"

namespace utils {
    void Reducer::basicReduce() {
        bool doIt;
        do {
            doIt = false;
            // this coresponds to rules 1.1-1.6 from Leitsch
            // 1.1) break IMPLICATION on conjunctions
            doIt |= reduceImplicationStep(parseTree.Root);
            // 1.2) push NOT further on conjunctions
            // 1.3) push NOT further on disjunctions
            // 1.4) NOT NOT gets reducted
            // 1.5) NOT@xF => ?xNOTF
            // 1.6) NOT?xF => @xNOTF
            doIt |= pushNOTStep(parseTree.Root);
        }while(doIt);
    }

    // does step 1.1)
    bool Reducer::reduceImplicationStep(int node) {
        throw "Not implemented";
    }

    // does step 1.2) - 1.6)
    bool Reducer::pushNOTStep(int node) {
        throw  "Not implemented";
    }

    void Reducer::skolemization() {
        while(skolemizationStep(parseTree.Root));
    }

    void Reducer::convertToCNF(int node) {
        while(convertToCNFStep(parseTree.Root));
    }

    bool Reducer::skolemizationStep(int node) {
        return false;
    }
}
