//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_PARSE_TREE_H
#define PROJECT_PARSE_TREE_H

#include "entity.h"
#include "reducer.h"
#include <string>
#include <vector>
#include <unordered_map>

namespace utils {
    class ParseTree {
        friend class Reducer;
    private:
        int Root;
        std::unordered_map <int, std::vector <int> > graph;
        std::unordered_map <int, Entity*> information;
        std::vector <int> spareNodesBuffer;
        int highestNodeLabel;
        /// when compacting/reducing nodes, push them here in order to reuse them later
        std::vector <int> redundantNodes;
        int getNextNode();
        void buildTree(const std::vector <std::string> &tokens);
        void applyParanthesesToConjunctions(int node);
    public:
        ParseTree()= default;
        explicit ParseTree(const std::vector <std::string> &tokens);
        string extractClauseForm();
    };
};

#endif //PROJECT_PARSE_TREE_H
