//
// Created by Patrick Sava on 11/4/2020.
//

#include "parse_tree.h"
#include <stack>
#include "verifier.h"
#include "operators.h"
#include "tokenizer.h"

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
        if (!Verifier::isBalanced(tokens)) {
            throw invalid_argument("the tokens given are not correctly parenthesised");
        }
        stack<int> fatherChain;
        fatherChain.push(Root);
        Operators& operators = Operators::getInstance();
        for (auto &token: tokens) {
            if (operators.whichOperator(0, token) != "none") {
                if (token == operators.OPENEDBracket or token == operators.NOT) {
                    auto node = getNextNode();
                    graph[fatherChain.top()].emplace_back(node);
                    fatherChain.push(node);
                    if (token == operators.NOT) {
                        information[node] = new Entity(EntityType::SIMPLIFIEDOperator, token);
                    }
                } else if (token == operators.CLOSEDBracket) {
                    fatherChain.pop();
                } else if (operators.isQuantifier(token)) {
                    auto node = getNextNode();
                    graph[fatherChain.top()].emplace_back(node);
                    fatherChain.push(node);
                    information[node] = new Entity(EntityType::BOUNDVariable, token);
                }
                else {
                    if (token != operators.AND and token != operators.IMPLY and token != operators.OR) {
                        throw invalid_argument("malformed tokens --- exhausted all of the possibilities for operators");
                    }
                    auto node = getNextNode();
                    graph[fatherChain.top()].emplace_back(node);
                    information[node] = new Entity(EntityType::SIMPLIFIEDOperator, token);
                }
            }
            else {
                // otherwise it should be predicate
                if (islower(token[0])) {
                    throw invalid_argument("Predicates start always with upper-case letter");
                }
                auto node = getNextNode();
                graph[fatherChain.top()].emplace_back(node);
                auto predicate = Tokenizer::decomposePredicate(token);
                information[node] = new Entity(EntityType::LITERAL, Literal(false, predicate.first, predicate.second));
            }
        }
    }

    void ParseTree::applyParanthesesToConjunctions(int node) {

    }

    string ParseTree::extractClauseForm() {
        string result;
        result += '{';

    }
}

