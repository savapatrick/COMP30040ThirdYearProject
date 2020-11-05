//
// Created by Patrick Sava on 11/4/2020.
//

#include "parse_tree.h"
#include <stack>

using namespace std;

namespace utils {
    ParseTree::ParseTree(const std::vector<std::string> &tokens) {
        Root = 1;
        const int bufferOrder = 100;
        highestNodeLabel = 100;
        for (int nodeLabel = 2; nodeLabel <= bufferOrder; ++ nodeLabel) {
            spareNodesBuffer.push_back(nodeLabel);
        }
        buildTree(tokens);
        applyParanthesesToConjunctions(Root);
    }

    int ParseTree::getNextNode() {
        auto takeNode = [&](vector <int> &nodes) -> int {
            auto result = nodes.back();
            nodes.pop_back();
            return result;
        };
        if (!spareNodesBuffer.empty()) {
            return takeNode(spareNodesBuffer);
        }
        while(!redundantNodes.empty()) {
            spareNodesBuffer.push_back(redundantNodes.back());
            redundantNodes.pop_back();
        }
        if (!spareNodesBuffer.empty()) {
            const int bufferOrder = 100;
            for (int nodeLabel = highestNodeLabel + 1; nodeLabel <= highestNodeLabel + 100; ++ nodeLabel) {
                spareNodesBuffer.push_back(nodeLabel);
            }
            highestNodeLabel += bufferOrder;
        }
        return takeNode(spareNodesBuffer);
    }

    void ParseTree::buildTree(const vector<std::string> &tokens) {
        stack<int> fatherChain;
        fatherChain.push(Root);
        for (auto &token: tokens) {

        }
    }

    void ParseTree::applyParanthesesToConjunctions(int node) {

    }
}

