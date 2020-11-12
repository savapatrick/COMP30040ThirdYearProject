//
// Created by Patrick Sava on 11/4/2020.
//

#include "reducer.h"
#include "operators.h"
#include <algorithm>

using namespace std;

namespace utils {

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

    int Reducer::addNodeWithOperator(const string &which) {
        Operators& operators = Operators::getInstance();
        auto newNode = parseTree.getNextNode();
        auto givenOperator = operators.getOperator(which);
        if (operators.isQuantifier(givenOperator)) {
            throw invalid_argument("given operator is a quantifier --- something went wrong");
        }
        parseTree.information[newNode] = new Entity(EntityType::SIMPLIFIEDOperator, givenOperator);
        return newNode;
    }

    int Reducer::addImplication(const int& nodeOne, const int& nodeTwo) {
        auto implication = addNodeWithOperator("IMPLY");
        auto father = parseTree.getNextNode();
        parseTree.graph[father].emplace_back(nodeOne);
        parseTree.graph[father].emplace_back(implication);
        parseTree.graph[father].emplace_back(nodeTwo);
        return father;
    }

    bool Reducer::applyParanthesesToOperators(int node,
                                                const std::string &targetOperator,
                                                const std::vector<std::string>& lowerOperators) {
        static vector<int> pile;
        Operators& operators = Operators::getInstance();
        pile.clear();
        bool andInside = false;
        int begin = 0;
        for (auto &neigh: parseTree.graph[node]) {
            pile.push_back(neigh);
            if (parseTree.information.find(neigh) != parseTree.information.end() and
                    parseTree.information[neigh]->getType() == EntityType::SIMPLIFIEDOperator) {
                if (operators.whichOperator(0, parseTree.information[neigh]->getString()) == targetOperator) {
                    if (!andInside) {
                        andInside = true;
                        if (pile.size() < 2) {
                            throw logic_error("Expected predicate before AND on the pile");
                        }
                        begin = (int) pile.size() - 2;
                    }
                }
                else if (any_of(lowerOperators.begin(), lowerOperators.end(), [&](const std::string &current) {
                    return current == operators.whichOperator(0, parseTree.information[neigh]->getString());
                }))
                {
                    if (andInside) {
                        auto newNode = parseTree.getNextNode();
                        for (int ind = begin; ind < (int) pile.size() - 2; ++ind) {
                            parseTree.graph[newNode].push_back(pile[ind]);
                        }
                        auto keep = pile.back();
                        pile.pop_back();
                        pile.push_back(newNode);
                        pile.push_back(keep);
                        andInside = false;
                    }
                }
                else if (operators.whichOperator(0, parseTree.information[neigh]->getString()) != "none") {
                    throw invalid_argument("the reduction of paranthesis went wrong because there are still"
                                           "operators which are having higher priority than " + targetOperator);
                }
            }
        }
        bool wasModified = false;
        wasModified |= (parseTree.graph[node] != pile);
        parseTree.graph[node] = pile;
        pile.clear();
        for (auto &neigh : parseTree.graph[node]) {
            wasModified |= applyParanthesesToOperators(neigh, targetOperator, lowerOperators);
        }
        return wasModified;
    }

    bool Reducer::applyParanthesesToConjunctions(int node) {
        return applyParanthesesToOperators(node, "AND", {"OR", "IMPLY", "DOUBLEImply"});
    }

    bool Reducer::applyParanthesesToDisjunctions(int node) {
        return applyParanthesesToOperators(node, "OR", {"IMPLY", "DOUBLEImply"});
    }

    bool Reducer::applyParanthesesToImplications(int node) {
        return applyParanthesesToOperators(node, "IMPLY", {"DOUBLEImply"});
    }

    bool Reducer::eliminateDoubleImplication(int node) {
        static vector<int> pile;
        Operators& operators = Operators::getInstance();
        pile.clear();
        for (auto &neigh: parseTree.graph[node]) {
            pile.push_back(neigh);
            if (pile.size() >= 3 and
            parseTree.information.find(pile[(int)pile.size() - 2]) != parseTree.information.end()) {
                if (operators.whichOperator(0, parseTree.information[pile[(int)pile.size() - 2]]->getString()) == "DOUBLEImply") {
                    auto rightPredicate = pile.back(); pile.pop_back();
                    auto doubleImply = pile.back(); pile.pop_back();
                    auto leftPredicate = pile.back(); pile.pop_back();
                    disposeNode(doubleImply);
                    pile.push_back(addImplication(leftPredicate, rightPredicate));
                    pile.push_back(addNodeWithOperator("AND"));
                    pile.push_back(addImplication(rightPredicate, leftPredicate));
                }
            }
        }
        bool wasModified = false;
        wasModified |= (parseTree.graph[node] != pile);
        parseTree.graph[node] = pile;
        pile.clear();
        for (auto &neigh : parseTree.graph[node]) {
            wasModified |= eliminateDoubleImplication(neigh);
        }
        return wasModified;
    }

    // warm-up for step 1.1)
    bool Reducer::reduceDoubleImplicationStep(int node) {
        if (applyParanthesesToConjunctions(node)) {
            if (applyParanthesesToConjunctions(node)) {
                throw logic_error(
                        "it should not get modified twice when "
                        "applying applyParanthesesToConjunctions");
            }
        }
        if (applyParanthesesToDisjunctions(node)) {
            if (applyParanthesesToDisjunctions(node)) {
                throw logic_error(
                        "it should not get modified twice when "
                        "applying applyParanthesesToDisjunctions");
            }
        }
        if (applyParanthesesToImplications(node)) {
            if (applyParanthesesToImplications(node)) {
                throw logic_error(
                        "it should not get modified twice when "
                        "applying applyParanthesesToImplications");
            }
        }
        if (eliminateDoubleImplication(node)) {
            if (eliminateDoubleImplication(node)) {
                throw logic_error(
                        "it should not get modified twice when "
                        "applying eliminateDoubleImplication");
            }
            return true;
        }
        return false;
    }

    bool Reducer::resolveRightAssociativityForImplications(int node) {
        return false;
    }

    // does step 1.1)
    bool Reducer::reduceImplicationStep(int node) {
        while (!reduceDoubleImplicationStep(node));
        if (resolveRightAssociativityForImplications(node)) {
            if (resolveRightAssociativityForImplications(node)) {
                throw logic_error(
                        "it should not get modified twice when "
                        "applying resolveRightAssociativityForImplications");
            }
        }
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

    void Reducer::convertToCNF() {
        while(convertToCNFStep(parseTree.Root));
    }

    std::string Reducer::extractClauseForm() {
        string result;
        result += "{";
        Operators& operators = Operators::getInstance();
        for (int ind = 0; ind < (int)parseTree.graph[parseTree.Root].size(); ++ ind) {
            auto child = parseTree.graph[parseTree.Root][ind];
            if (ind % 2 == 0) {
                if (parseTree.information[child]->getType() != EntityType::NORMALForms) {
                    throw logic_error("malformed tree --- cannot extract it in clause form");
                }
                else {
                    auto entity = parseTree.information[child]->getEntity<Entity::NormalFormStorage>();
                    if (entity.first != NormalFormType::DNF) {
                        throw logic_error("malformed tree --- cannot extract it in clause form");
                    }
                }
            }
            else {
                if (parseTree.information[child]->getType() != EntityType::SIMPLIFIEDOperator) {
                    throw logic_error("malformed tree --- cannot extract it in clause form");
                }
                else {
                    if (operators.whichOperator(0, parseTree.information[child]->getString()) != "AND") {
                        throw logic_error("malformed tree --- cannot extract it in clause form");
                    }
                }
            }
            result += parseTree.information[child]->getString();
            if (ind + 1 == parseTree.graph[parseTree.Root].size()) {
                result += "}";
            }
            else {
                result += ", ";
            }
        }
        return result;
    }

    string Reducer::getClauseForm() {
        basicReduce();
        skolemization();
        convertToCNF();
        return extractClauseForm();
    }
}
