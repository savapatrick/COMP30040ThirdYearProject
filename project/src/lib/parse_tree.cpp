//
// Created by Patrick Sava on 11/4/2020.
//

#include "parse_tree.h"
#include "operators.h"
#include "tokenizer.h"
#include "verifier.h"
#include <algorithm>
#include <memory>
#include <stack>

using namespace std;

namespace utils {
ParseTree::ParseTree(const std::string& formula) {
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    auto tokens                 = tokenizer.tokenize(formula);
    graph.clear();
    information.clear();
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
    if(!spareNodesBuffer.empty()) {
        const int bufferOrder = 100;
        for(int nodeLabel = highestNodeLabel + 1; nodeLabel <= highestNodeLabel + 100; ++nodeLabel) {
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
            auto predicate                   = Tokenizer::decomposePredicate(token);
            std::shared_ptr<Literal> literal = std::make_shared<Literal>(false, predicate.first, predicate.second);
            information[node]                = make_shared<Entity>(EntityType::LITERAL, literal);
            while(!operatorPrecedenceNOTQuant.empty() and sumSoFarParanthesis == operatorPrecedenceNOTQuant.top().second) {
                auto target = operatorPrecedenceNOTQuant.top().first;
                operatorPrecedenceNOTQuant.pop();
                while(fatherChain.size() > target) { fatherChain.pop(); }
            }
        }
    }
}

std::string ParseTree::getEulerTraversal(int node) {
    string result;
    if(information.find(node) != information.end()) {
        result += information[node]->getString();
    } else {
        result += "$none$";
    }
    if(graph.find(node) != graph.end()) {
        for(auto& neighbour : graph[node]) { result += getEulerTraversal(neighbour); }
    }
    if(information.find(node) != information.end()) {
        result += information[node]->getString();
    } else {
        result += "$none$";
    }
    return result;
}

std::string ParseTree::getEulerTraversal() {
    return getEulerTraversal(Root);
}
} // namespace utils
