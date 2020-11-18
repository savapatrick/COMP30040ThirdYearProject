//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_PARSE_TREE_H
#define PROJECT_PARSE_TREE_H

#include "entity.h"
#include "reducer.h"
#include <string>
#include <unordered_map>
#include <vector>

namespace utils {
class ParseTree {
    friend class Reducer;

    private:
    int Root;
    std::unordered_map<int, std::vector<int>> graph;
    std::unordered_map<int, std::shared_ptr<Entity>> information;
    std::vector<int> spareNodesBuffer;
    int highestNodeLabel;
    /// when compacting/reducing nodes, push them here in order to reuse them later
    std::vector<int> redundantNodes;

    int getNextNode ();

    void buildTree (const std::vector<std::string>& tokens);

    std::string getEulerTraversal (int node);

    public:
    ParseTree () = default;

    explicit ParseTree (const std::string& tokens);

    std::string getEulerTraversal ();
};
}; // namespace utils

#endif // PROJECT_PARSE_TREE_H
