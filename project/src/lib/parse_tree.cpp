//
// Created by Patrick Sava on 11/4/2020.
//

#include "parse_tree.h"
#include "ad_hoc_templated.h"
#include "operators.h"
#include "tokenizer.h"
#include "verifier.h"
#include <algorithm>
#include <memory>
#include <stack>
#include <iostream>

using namespace std;

namespace utils {
ParseTree::ParseTree(const std::string& formula) {
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    auto tokens                 = utils::Tokenizer::tokenize(formula);
    graph.clear();
    information.clear();
    fakeNode.clear();
    spareNodesBuffer.clear();
    redundantNodes.clear();
    Root                  = 1;
    const int bufferOrder = 100;
    highestNodeLabel      = 100;
    for(int nodeLabel = 2; nodeLabel <= bufferOrder; ++nodeLabel) { spareNodesBuffer.push_back(nodeLabel); }
    buildTree(tokens);
}
ParseTree::ParseTree(const vector<std::string>& formulas) {
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    string formula;
    Operators& operators = Operators::getInstance();
    for(int index = 0; index < (int)formulas.size(); ++index) {
        auto currentFormula = formulas[index];
        if(index + 1 != (int)formulas.size()) {
            formula += operators.OPENEDBracket + currentFormula + operators.CLOSEDBracket + operators.AND;
        } else {
            formula += operators.OPENEDBracket + operators.NOT + operators.OPENEDBracket + currentFormula +
            operators.CLOSEDBracket + operators.CLOSEDBracket;
        }
    }
    auto tokens = utils::Tokenizer::tokenize(formula);
    graph.clear();
    information.clear();
    fakeNode.clear();
    spareNodesBuffer.clear();
    redundantNodes.clear();
    Root                  = 1;
    const int bufferOrder = 100;
    highestNodeLabel      = 100;
    for(int nodeLabel = 2; nodeLabel <= bufferOrder; ++nodeLabel) { spareNodesBuffer.push_back(nodeLabel); }
    buildTree(tokens);
}

int ParseTree::getNextNode() {
    auto takeNode = [&](vector<int>& nodes) -> int {
        auto result = nodes.back();
        nodes.pop_back();
        return result;
    };
    if(!spareNodesBuffer.empty()) {
        return takeNode(spareNodesBuffer);
    }
    while(!redundantNodes.empty()) {
        spareNodesBuffer.push_back(redundantNodes.back());
        redundantNodes.pop_back();
    }
    if(spareNodesBuffer.empty()) {
        const int bufferOrder = 1000;
        for(int nodeLabel = highestNodeLabel + 1; nodeLabel <= highestNodeLabel + bufferOrder; ++nodeLabel) {
            spareNodesBuffer.push_back(nodeLabel);
        }
        highestNodeLabel += bufferOrder;
    }
    return takeNode(spareNodesBuffer);
}

void ParseTree::buildTree(const vector<std::string>& tokens) {
    /// operator precedence
    /// NOT, QUANTIFIER, AND, OR, IMPLICATION, DOUBLE IMPLICATION
    if(!Verifier::isBalanced(tokens)) {
        throw invalid_argument("the tokens given are not correctly parenthesised");
    }
    stack<int> fatherChain;
    fatherChain.push(Root);
    Operators& operators    = Operators::getInstance();
    int sumSoFarParanthesis = 0;
    stack<pair<int, int>> operatorPrecedenceNOTQuant;
    for(auto& token : tokens) {
        if(operators.whichOperator(0, token) != "none") {
            if(token == operators.OPENEDBracket or token == operators.NOT) {
                auto node = getNextNode();
                graph[fatherChain.top()].emplace_back(node);
                fatherChain.push(node);
                if(token == operators.NOT) {
                    information[node] = make_shared<Entity>(EntityType::SIMPLIFIEDOperator, token);
                    operatorPrecedenceNOTQuant.push({ (int)fatherChain.size() - 1, sumSoFarParanthesis });
                } else {
                    sumSoFarParanthesis += 1;
                }
            } else if(token == operators.CLOSEDBracket) {
                fatherChain.pop();
                sumSoFarParanthesis -= 1;
                while(!operatorPrecedenceNOTQuant.empty() and sumSoFarParanthesis == operatorPrecedenceNOTQuant.top().second) {
                    auto target = operatorPrecedenceNOTQuant.top().first;
                    operatorPrecedenceNOTQuant.pop();
                    while(fatherChain.size() > target) { fatherChain.pop(); }
                }
            } else if(operators.isQuantifierAndVariable(token)) {
                auto node = getNextNode();
                graph[fatherChain.top()].emplace_back(node);
                fatherChain.push(node);
                information[node] = make_shared<Entity>(EntityType::BOUNDVariable, token);
                operatorPrecedenceNOTQuant.push({ (int)fatherChain.size() - 1, sumSoFarParanthesis });
            } else {
                if(token != operators.AND and token != operators.IMPLY and token != operators.OR and token != operators.DOUBLEImply) {
                    throw invalid_argument(
                    "malformed tokens --- exhausted all of the possibilities for operators; token is " + token);
                }
                auto node = getNextNode();
                graph[fatherChain.top()].emplace_back(node);
                information[node] = make_shared<Entity>(EntityType::SIMPLIFIEDOperator, token);
            }
        } else {
            // otherwise it should be predicate
            if(islower(token[0])) {
                throw invalid_argument("Predicates start always with upper-case letter");
            }
            auto node = getNextNode();
            graph[fatherChain.top()].emplace_back(node);
            auto predicate = Tokenizer::decomposePredicate(token);
            std::shared_ptr<SimplifiedLiteral> simplifiedLiteral =
            std::make_shared<SimplifiedLiteral>(false, predicate.first, predicate.second);
            information[node] = make_shared<Entity>(EntityType::SIMPLIFIEDLiteral, simplifiedLiteral);
            while(!operatorPrecedenceNOTQuant.empty() and sumSoFarParanthesis == operatorPrecedenceNOTQuant.top().second) {
                auto target = operatorPrecedenceNOTQuant.top().first;
                operatorPrecedenceNOTQuant.pop();
                while(fatherChain.size() > target) { fatherChain.pop(); }
            }
        }
    }
}

std::string ParseTree::getEulerTraversal(int node, bool isLabeled = false) {
    string result;
    if(information.find(node) != information.end()) {
        result += information[node]->getString() + (isLabeled ? to_string(node) : "");
    } else {
        result += "$none$" + (isLabeled ? to_string(node) : "");
    }
    if(graph.find(node) != graph.end()) {
        for(auto& neighbour : graph[node]) { result += getEulerTraversal(neighbour, isLabeled); }
    }
    if(information.find(node) != information.end()) {
        result += information[node]->getString() + (isLabeled ? to_string(node) : "");
    } else {
        result += "$none$" + (isLabeled ? to_string(node) : "");
    }
    return result;
}

std::string ParseTree::getEulerTraversal() {
    return getEulerTraversal(Root);
}

int ParseTree::createCopyForSubtree(int node) {
    int newNode = getNextNode();
    if(information.find(node) != information.end()) {
        information[newNode] = make_shared<Entity>(information[node]);
    }
    graph[newNode].clear();
    graph[newNode].reserve(graph[node].size());
    for(auto& neighbour : graph[node]) { graph[newNode].push_back(createCopyForSubtree(neighbour)); }
    return newNode;
}

int ParseTree::addNodeWithOperator(const string& which) {
    Operators& operators = Operators::getInstance();
    auto newNode         = getNextNode();
    auto givenOperator   = operators.getOperator(which);
    if(operators.isQuantifier(givenOperator)) {
        throw invalid_argument("given operator is a quantifier --- something went wrong");
    }
    information[newNode] = make_shared<Entity>(EntityType::SIMPLIFIEDOperator, givenOperator);
    return newNode;
}

int ParseTree::addImplication(const int& nodeOne, const int& nodeTwo) {
    auto implication = addNodeWithOperator("IMPLY");
    auto father      = getNextNode();
    graph[father].emplace_back(nodeOne);
    graph[father].emplace_back(implication);
    graph[father].emplace_back(nodeTwo);
    return father;
}

int ParseTree::addDoubleImplication(const int& nodeOne, const int& nodeTwo) {
    auto doubleImplication = addNodeWithOperator("DOUBLEImply");
    auto father      = getNextNode();
    graph[father].emplace_back(nodeOne);
    graph[father].emplace_back(doubleImplication);
    graph[father].emplace_back(nodeTwo);
    return father;
}

int ParseTree::addOrClause(const int& nodeOne, const int& nodeTwo) {
    auto orOperator = addNodeWithOperator("OR");
    auto father     = getNextNode();
    graph[father].emplace_back(nodeOne);
    graph[father].emplace_back(orOperator);
    graph[father].emplace_back(nodeTwo);
    return father;
}

int ParseTree::addAndClause(const int& nodeOne, const int& nodeTwo) {
    auto orOperator = addNodeWithOperator("AND");
    auto father     = getNextNode();
    graph[father].emplace_back(nodeOne);
    graph[father].emplace_back(orOperator);
    graph[father].emplace_back(nodeTwo);
    return father;
}

int ParseTree::addNegationToFormula(const int& nodeOne) {
    auto notOperator = addNodeWithOperator("NOT");
    graph[notOperator].emplace_back(nodeOne);
    if(nodeOne == Root) {
        Root = getNextNode();
        graph[Root].emplace_back(notOperator);
        return Root;
    }
    return notOperator;
}
void ParseTree::disposeNode(int node) {
    try {
        graph.erase(graph.find(node));
        information.erase(information.find(node));
        redundantNodes.push_back(node);
    } catch(...) { throw invalid_argument("dispose node failed to dispose node " + to_string(node)); }
}
} // namespace utils
