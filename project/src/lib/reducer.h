//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_REDUCER_H
#define PROJECT_REDUCER_H

#include "parse_tree.h"
#include "entity.h"

namespace utils {
    class ParseTree;
    class Reducer {
    private:
        ParseTree& parseTree;
        void disposeNode(int node);
        bool reduceImplicationStep(int node);
        bool pushNOTStep(int node);
        bool skolemizationStep(int node);
        bool convertToCNFStep(int node);
        Entity mergeSameNormalFormEntities(const Entity& first, const Entity& second);
//        Entity mergeNormalFormEntitiesOnOrO
    public:
        Reducer()= default;
        explicit Reducer(ParseTree &_parseTree) : parseTree(_parseTree){}
        void basicReduce();
        void skolemization();
        void convertToCNF(int node);
    };
};

#endif //PROJECT_REDUCER_H
