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

    int getNextNode();

    void buildTree(const std::vector<std::string>& tokens);

    std::string getEulerTraversal(int node, bool isLabeled);
    std::string inOrderTraversal(int node);

    int addNodeWithOperator(const std::string& which);
    int addNodeWithBoundedVariable(const std::string& variableName, bool isUniversal);
    int addFatherWithUniversallyBoundedVariable(const int& node, const std::string& variableName);

    int addDoubleImplication(const int& nodeOne, const int& nodeTwo);
    int addImplication(const int& nodeOne, const int& nodeTwo);

    int addOrClause(const int& nodeOne, const int& nodeTwo);
    int addAndClause(const int& nodeOne, const int& nodeTwo);

    int addNegationToFormula(const int& nodeOne);

    void disposeNode(int node);

    public:
    ParseTree() = default;

    explicit ParseTree(const std::string& tokens);
    explicit ParseTree(const std::vector<std::string>& tokens);

    std::string getEulerTraversal();

    int createCopyForSubtree(int node);
};
}; // namespace utils

#endif // PROJECT_PARSE_TREE_H
