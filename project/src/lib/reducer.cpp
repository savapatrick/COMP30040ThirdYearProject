//
// Created by Patrick Sava on 11/4/2020.
//

#include "reducer.h"
#include <algorithm>

using namespace std;

namespace utils {

    // does step 1.1)
    bool Reducer::reduceImplicationStep(int node) {
        throw "Not implemented";
    }

    // does step 1.2) - 1.6)
    bool Reducer::pushNOTStep(int node) {
        throw  "Not implemented";
    }

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

    bool Reducer::skolemizationStep(int node) {
        return false;
    }

    void Reducer::skolemization() {
        while(skolemizationStep(parseTree.Root));
    }

    Entity Reducer::mergeSameNormalFormEntities(const Entity &first, const Entity &second) {
        if (first.getType() == NORMALForms and second.getType() == NORMALForms) {
            auto firstStorage = first.getEntity<Entity::NormalFormStorage>();
            auto secondStorage = second.getEntity<Entity::NormalFormStorage>();
            if(firstStorage.first == secondStorage.first) {
                vector<Literal> result(first.getEntity<Entity::NormalFormStorage>().second);
                vector<Literal> aux(second.getEntity<Entity::NormalFormStorage>().second);
                for (auto &elem: aux) {
                    result.emplace_back(elem);
                }
                return Entity(NORMALForms, Entity::NormalFormStorage(firstStorage.first, result));
            }
            else {
                throw invalid_argument("Both of the Entities provided have to be in CNF(both) or in DNF(both)");
            }
        }
        throw invalid_argument("at least of the Entities provided are not in NormalForm");
    }

    bool Reducer::convertToCNFStep(int node) {
        return false;
    }

    void Reducer::convertToCNF(int node) {
        while(convertToCNFStep(parseTree.Root));
    }

    void Reducer::disposeNode(int node) {
        try {
            parseTree.graph.erase(parseTree.graph.find(node));
            delete parseTree.information[node];
            parseTree.information.erase(parseTree.information.find(node));
            parseTree.redundantNodes.push_back(node);
        } catch (...) {
            throw invalid_argument("dispose node failed to dispose node " + to_string(node));
        }
    }
}
