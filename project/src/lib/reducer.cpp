//
// Created by Patrick Sava on 11/4/2020.
//

#include "reducer.h"
#include "operators.h"
#include <algorithm>
#include <cassert>

using namespace std;

namespace utils {

    void Reducer::disposeNode(int node) {
        try {
            parseTree.graph.erase(parseTree.graph.find(node));
            parseTree.information.erase(parseTree.information.find(node));
            parseTree.redundantNodes.push_back(node);
        } catch (...) {
            throw invalid_argument("dispose node failed to dispose node " + to_string(node));
        }
    }

    std::shared_ptr<Entity> Reducer::getEntityWithFlippedQuantifierAndVariable(const string &which) {
        Operators& operators = Operators::getInstance();
        auto givenQuantifier = operators.getQuantifierFromQuantifierAndVariable(which);
        auto givenVariable = operators.getVariableFromQuantifierAndVariable(which);
        auto newQuantifierAndVariable = operators.flipQuantifier(givenQuantifier) + givenVariable;
        return make_shared<Entity>(EntityType::BOUNDVariable, newQuantifierAndVariable);
    }

    int Reducer::addNodeWithOperator(const string &which) {
        Operators& operators = Operators::getInstance();
        auto newNode = parseTree.getNextNode();
        auto givenOperator = operators.getOperator(which);
        if (operators.isQuantifier(givenOperator)) {
            throw invalid_argument("given operator is a quantifier --- something went wrong");
        }
        parseTree.information[newNode] = make_shared<Entity>(EntityType::SIMPLIFIEDOperator, givenOperator);
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

    int Reducer::addOrClause(const int& nodeOne, const int& nodeTwo) {
        auto orOperator = addNodeWithOperator("OR");
        auto father = parseTree.getNextNode();
        parseTree.graph[father].emplace_back(nodeOne);
        parseTree.graph[father].emplace_back(orOperator);
        parseTree.graph[father].emplace_back(nodeTwo);
        return father;
    }

    int Reducer::addNegationToFormula(const int& nodeOne) {
        auto notOperator = addNodeWithOperator("NOT");
        parseTree.graph[notOperator].emplace_back(nodeOne);
        return notOperator;
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

    bool Reducer::eliminateDoubleImplicationOrImplication(bool isDoubleImplication, int node) {
        static vector<int> pile;
        Operators& operators = Operators::getInstance();
        pile.clear();
        if (!isDoubleImplication) {
            /// that's because it's implication and this is
            /// RIGHT ASSOCIATIVE
            reverse(parseTree.graph[node].begin(), parseTree.graph[node].end());
        }
        for (auto &neigh: parseTree.graph[node]) {
            pile.push_back(neigh);
            if (pile.size() >= 3 and
            parseTree.information.find(pile[(int)pile.size() - 2]) != parseTree.information.end()) {
                auto whichOperator = operators.whichOperator(0, parseTree.information[pile[(int)pile.size() - 2]]->getString());
                if (isDoubleImplication and whichOperator == "DOUBLEImply") {
                    auto rightPredicate = pile.back(); pile.pop_back();
                    auto doubleImply = pile.back(); pile.pop_back();
                    auto leftPredicate = pile.back(); pile.pop_back();
                    disposeNode(doubleImply);
                    pile.push_back(addImplication(leftPredicate, rightPredicate));
                    pile.push_back(addNodeWithOperator("AND"));
                    pile.push_back(addImplication(rightPredicate, leftPredicate));
                }
                else if (!isDoubleImplication and whichOperator == "IMPLY") {
                    auto leftPredicate = pile.back(); pile.pop_back();
                    auto imply = pile.back(); pile.pop_back();
                    auto rightPredicate = pile.back(); pile.pop_back();
                    disposeNode(imply);
                    auto negatedLeftPredicate = addNegationToFormula(leftPredicate);
                    pile.push_back(addOrClause(negatedLeftPredicate, rightPredicate));
                }
            }
        }
        bool wasModified = false;
        wasModified |= (parseTree.graph[node] != pile);
        if (!isDoubleImplication) {
            // this is because we have to revert the correct order for implication
            reverse(pile.begin(), pile.end());
        }
        parseTree.graph[node] = pile;
        pile.clear();
        for (auto &neigh : parseTree.graph[node]) {
            wasModified |= eliminateDoubleImplicationOrImplication(isDoubleImplication, neigh);
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
        if (eliminateDoubleImplicationOrImplication(true, node)) {
            if (eliminateDoubleImplicationOrImplication(true, node)) {
                throw logic_error(
                        "it should not get modified twice when "
                        "applying eliminateDoubleImplication");
            }
            return true;
        }
        return false;
    }

    bool Reducer::resolveRightAssociativityForImplications(int node) {
        return eliminateDoubleImplicationOrImplication(false, node);
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
            return true;
        }
        return false;
    }

    // does step 1.2) - 1.6)
    bool Reducer::pushNOTStep(int node) {
        bool isNot = false;
        Operators& operators = Operators::getInstance();
        if (parseTree.information.find(node) != parseTree.information.end()) {
            if (operators.whichOperator(0, parseTree.information[node]->getString()) == "NOT") {
                isNot = true;
            }
        }
        string operatorOnTheLevel;
        for (auto &neighbour: parseTree.graph[node]) {
            if (parseTree.information.find(node) != parseTree.information.end()) {
                if(parseTree.information[node]->getType() == EntityType::SIMPLIFIEDOperator) {
                    if (operatorOnTheLevel.empty()) {
                        operatorOnTheLevel = parseTree.information[node]->getString();
                        if (operatorOnTheLevel == "NOT") {
                            // that's not unary
                            // then reset
                            operatorOnTheLevel.clear();
                        }
                        else if (operatorOnTheLevel != "AND" and operatorOnTheLevel != "OR") {
                            // at this point the tree should have only AND, OR and NOT + quantifiers
                            throw logic_error("at this point the tree should have only AND, OR and NOT + quantifiers");
                        }
                    }
                    else {
                        if (operatorOnTheLevel != parseTree.information[node]->getString()) {
                            throw logic_error("all of the simplified operators which are on"
                                              "the same level should be the same at this point");
                        }
                    }
                }
            }
        }
        bool wasModified = false;
        if (isNot) {
            vector <int> newNodes;
            /// at this point we know that all what we have is either a CNF or a DNF
            for (auto &neighbour: parseTree.graph[node]) {
                newNodes.push_back(neighbour);
                if (parseTree.information.find(neighbour) != parseTree.information.end()) {
                    switch (parseTree.information[neighbour]->getType()) {
                        case BOUNDVariable: {
                            newNodes.pop_back();
                            if (operators.isQuantifierAndVariable(
                                    parseTree.information[neighbour]->getString())) {
                                auto newEntity = getEntityWithFlippedQuantifierAndVariable(
                                parseTree.information[neighbour]->getString()
                                );
                                parseTree.information[neighbour] = parseTree.information[node];
                                auto newNode = parseTree.getNextNode();
                                parseTree.information[newNode] = newEntity;
                                parseTree.graph[newNode].push_back(neighbour);
                                newNodes.push_back(newNode);
                            }
                            else {
                                throw logic_error("BOUNDVariable should always contain a quantifier "
                                                  "and a variable but it was given the "
                                                  "following " + parseTree.information[neighbour]->getString());
                            }
                            break;
                        }
                        case SIMPLIFIEDOperator: {
                            if (operators.whichOperator(
                                    0, parseTree.information[neighbour]->getString()) == "NOT") {
                                parseTree.information.erase(parseTree.information.find(neighbour));
                            }
                            else if (operators.isAnd(parseTree.information[neighbour]->getString())
                                or operators.isOr(parseTree.information[neighbour]->getString())) {
                                auto target = operators.flipAndOrOr(parseTree.information[neighbour]->getString());
                                parseTree.information[neighbour] = make_shared<Entity>(EntityType::SIMPLIFIEDOperator, target);
                            }
                            break;
                        }
                        case LITERAL:
                            /// TODO: reconsider what I am going to do here.
                            parseTree.information[neighbour]->getEntity<Literal*>()->negate();
                            break;
                        case NORMALForms:
                            break;
                        default:
                            /// if we reach this state, we can kill the program
                            /// because it's absurd
                            assert(false);
                    }
                }
                else {
                    parseTree.information[neighbour] = parseTree.information[node];
                }
            }
            wasModified = true;
            parseTree.information.erase(parseTree.information.find(node));
        }
        for (auto &neighbour: parseTree.graph[node]) {
            wasModified |= pushNOTStep(neighbour);
        }
        return wasModified;
    }

    void Reducer::basicReduce() {
        bool doIt;
        do {
            doIt = false;
            // this coresponds to rules 1.1-1.6 from Leitsch
            // 1.1) break IMPLICATION on disjunctions
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
