//
// Created by Patrick Sava on 11/4/2020.
//

#include "parse_tree.h"
#include "verifier.h"
#include "operators.h"
#include "tokenizer.h"
#include <stack>
#include <algorithm>

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
        /// operator precedence
        /// NOT, AND, OR, IMPLICATION
        if (!Verifier::isBalanced(tokens)) {
            throw invalid_argument("the tokens given are not correctly parenthesised");
        }
        stack<int> fatherChain;
        fatherChain.push(Root);
        Operators& operators = Operators::getInstance();
        int sumSoFarParanthesis = 0;
        stack<pair <int, int>> operatorPrecedenceNOTQuant;
        for (auto &token: tokens) {
            if (operators.whichOperator(0, token) != "none") {
                if (token == operators.OPENEDBracket or token == operators.NOT) {
                    auto node = getNextNode();
                    graph[fatherChain.top()].emplace_back(node);
                    fatherChain.push(node);
                    if (token == operators.NOT) {
                        information[node] = new Entity(EntityType::SIMPLIFIEDOperator, token);
                        operatorPrecedenceNOTQuant.push({(int)fatherChain.size() - 1, sumSoFarParanthesis});
                    }
                    else {
                        sumSoFarParanthesis += 1;
                    }
                } else if (token == operators.CLOSEDBracket) {
                    fatherChain.pop();
                    sumSoFarParanthesis -= 1;
                    if (!operatorPrecedenceNOTQuant.empty() and sumSoFarParanthesis == operatorPrecedenceNOTQuant.top().second) {
                        auto target = operatorPrecedenceNOTQuant.top().first;
                        operatorPrecedenceNOTQuant.pop();
                        while(fatherChain.size() > target) {
                            fatherChain.pop();
                        }
                    }
                } else if (operators.isQuantifier(token)) {
                    auto node = getNextNode();
                    graph[fatherChain.top()].emplace_back(node);
                    fatherChain.push(node);
                    information[node] = new Entity(EntityType::BOUNDVariable, token);
                    operatorPrecedenceNOTQuant.push({(int)fatherChain.size() - 1, sumSoFarParanthesis});
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
                if (!operatorPrecedenceNOTQuant.empty() and sumSoFarParanthesis == operatorPrecedenceNOTQuant.top().second) {
                    auto target = operatorPrecedenceNOTQuant.top().first;
                    operatorPrecedenceNOTQuant.pop();
                    while(fatherChain.size() > target) {
                        fatherChain.pop();
                    }
                }
            }
        }
    }

    bool ParseTree::applyParanthesesToOperators(int node,
                                                const std::string &targetOperator,
                                                const std::vector<std::string>& lowerOperators) {
        static vector<int> pile;
        Operators& operators = Operators::getInstance();
        pile.clear();
        bool andInside = false;
        int begin = 0;
        for (auto &neigh: graph[node]) {
            pile.push_back(neigh);
            if (information.find(neigh) != information.end() and
            information[neigh]->getType() == EntityType::SIMPLIFIEDOperator) {
                if (operators.whichOperator(0, information[neigh]->getString()) == targetOperator) {
                    if (!andInside) {
                        andInside = true;
                        if (pile.size() < 2) {
                            throw logic_error("Expected predicate before AND on the pile");
                        }
                        begin = (int) pile.size() - 2;
                    }
                }
                else if (any_of(lowerOperators.begin(), lowerOperators.end(), [&](const std::string &current) {
                    return current == operators.whichOperator(0, information[neigh]->getString());
                }))
                {
                    if (andInside) {
                        auto newNode = getNextNode();
                        for (int ind = begin; ind < (int) pile.size() - 2; ++ind) {
                            graph[newNode].push_back(pile[ind]);
                        }
                        auto keep = pile.back();
                        pile.pop_back();
                        pile.push_back(newNode);
                        pile.push_back(keep);
                        andInside = false;
                    }
                }
            }
        }
        bool wasModified = false;
        wasModified |= (graph[node] != pile);
        graph[node] = pile;
        pile.clear();
        for (auto &neigh : graph[node]) {
            wasModified |= applyParanthesesToConjunctions(neigh);
        }
        return wasModified;
    }

    bool ParseTree::applyParanthesesToConjunctions(int node) {
        return applyParanthesesToOperators(node, "AND", {"OR", "IMPLY"});
    }

    bool ParseTree::applyParanthesesToDisjunctions(int node) {
        return applyParanthesesToOperators(node, "OR", {"IMPLY"});
    }

    string ParseTree::extractClauseForm() {
        string result;
        result += "{";
        Operators& operators = Operators::getInstance();
        for (int ind = 0; ind < (int)graph[Root].size(); ++ ind) {
            auto child = graph[Root][ind];
            if (ind % 2 == 0) {
                if (information[child]->getType() != EntityType::NORMALForms) {
                    throw logic_error("malformed tree --- cannot extract it in clause form");
                }
                else {
                    auto entity = information[child]->getEntity<Entity::NormalFormStorage>();
                    if (entity.first != NormalFormType::DNF) {
                        throw logic_error("malformed tree --- cannot extract it in clause form");
                    }
                }
            }
            else {
                if (information[child]->getType() != EntityType::SIMPLIFIEDOperator) {
                    throw logic_error("malformed tree --- cannot extract it in clause form");
                }
                else {
                    if (operators.whichOperator(0, information[child]->getString()) != "AND") {
                        throw logic_error("malformed tree --- cannot extract it in clause form");
                    }
                }
            }
            result += information[child]->getString();
            if (ind + 1 == graph[Root].size()) {
                result += "}";
            }
            else {
                result += ", ";
            }
        }
        return result;
    }
}

