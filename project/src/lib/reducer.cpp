//
// Created by Patrick Sava on 11/4/2020.
//

#include "reducer.h"
#include "operators.h"
#include <algorithm>
#include <cassert>
#include <ctime>
#include <iostream>

using namespace std;

namespace utils {

Reducer::Reducer(ParseTree& _parseTree) : parseTree(_parseTree) {
    Operators& operators = Operators::getInstance();
    for(auto& info : parseTree.information) {
        auto value = info.second;
        if(value->getType() == EntityType::SIMPLIFIEDLiteral) {
            auto simplifiedLiteral = value->getEntity<shared_ptr<SimplifiedLiteral>>();
            auto arguments         = simplifiedLiteral->getArguments();
            for(auto& argument : arguments) {
                if(argument.index() == 1) {
                    throw invalid_argument("given a non-raw parse tree to the Reducer constructor");
                }
                reservedVariableNames.insert(SimplifiedLiteral::getArgumentString(argument));
            }
            reservedPredicateNames.insert(simplifiedLiteral->getPredicateName());
        } else if(value->getType() == EntityType::BOUNDVariable) {
            reservedVariableNames.insert(operators.getVariableFromQuantifierAndVariable(value->getString()));
        } else if(value->getType() == EntityType::NORMALForms) {
            // for the intended purpose of this class, we should never enter on this branch
            // because we expect the Reducer class to always get the raw parseTree
            // TODO: consider whether to enforce that/redesign
            throw invalid_argument("Reducer always expects a raw parse tree; given one with normal forms");
        } else if(value->getType() == EntityType::SIMPLIFIEDOperator) {
            // do nothing
            continue;
        }
    }
}

std::string Reducer::getRandomFunctionOrConstantName() {
    static std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    const int sizeOfAlphabet    = 26;
    // TODO: consider whether we want the C++11 random generator
    std::srand(13);
    string result;
    do {
        result.clear();
        int length = rand() % 15 + 1; /// 26^15 is huge
        for(int ind = 1; ind <= length; ++ind) { result += alphabet[rand() % sizeOfAlphabet]; }
    } while(reservedVariableNames.find(result) != reservedVariableNames.end());
    reservedVariableNames.insert(result);
    return result;
}

std::string Reducer::getRandomPredicateName() {
    static std::string startingLetterAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::string alphabet               = "abcdefghijklmnopqrstuvwxyz";
    const int sizeOfAlphabet                  = 26;
    // TODO: consider whether we want the C++11 random generator
    std::srand(14);
    string result;
    do {
        result.clear();
        int length = rand() % 15 + 1; /// 26^15 is huge
        result += startingLetterAlphabet[rand() % sizeOfAlphabet];
        for(int ind = 2; ind <= length; ++ind) { result += alphabet[rand() % sizeOfAlphabet]; }
    } while(reservedPredicateNames.find(result) != reservedPredicateNames.end());
    reservedPredicateNames.insert(result);
    return result;
}

void Reducer::disposeNode(int node) {
    try {
        parseTree.graph.erase(parseTree.graph.find(node));
        parseTree.information.erase(parseTree.information.find(node));
        parseTree.redundantNodes.push_back(node);
    } catch(...) { throw invalid_argument("dispose node failed to dispose node " + to_string(node)); }
}

std::shared_ptr<Entity> Reducer::getEntityWithFlippedQuantifierAndVariable(const string& which) {
    Operators& operators          = Operators::getInstance();
    auto givenQuantifier          = operators.getQuantifierFromQuantifierAndVariable(which);
    auto givenVariable            = operators.getVariableFromQuantifierAndVariable(which);
    auto newQuantifierAndVariable = operators.flipQuantifier(givenQuantifier) + givenVariable;
    return make_shared<Entity>(EntityType::BOUNDVariable, newQuantifierAndVariable);
}

int Reducer::addNodeWithOperator(const string& which) {
    Operators& operators = Operators::getInstance();
    auto newNode         = parseTree.getNextNode();
    auto givenOperator   = operators.getOperator(which);
    if(operators.isQuantifier(givenOperator)) {
        throw invalid_argument("given operator is a quantifier --- something went wrong");
    }
    parseTree.information[newNode] = make_shared<Entity>(EntityType::SIMPLIFIEDOperator, givenOperator);
    return newNode;
}

int Reducer::addImplication(const int& nodeOne, const int& nodeTwo) {
    auto implication = addNodeWithOperator("IMPLY");
    auto father      = parseTree.getNextNode();
    parseTree.graph[father].emplace_back(nodeOne);
    parseTree.graph[father].emplace_back(implication);
    parseTree.graph[father].emplace_back(nodeTwo);
    return father;
}

int Reducer::addOrClause(const int& nodeOne, const int& nodeTwo) {
    auto orOperator = addNodeWithOperator("OR");
    auto father     = parseTree.getNextNode();
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

bool Reducer::applyParanthesesToOperators(int node, const std::string& targetOperator, const std::vector<std::string>& lowerOperators) {
    static vector<int> pile;
    Operators& operators = Operators::getInstance();
    pile.clear();
    bool andInside = false;
    int begin      = 0;
    for(auto& neigh : parseTree.graph[node]) {
        pile.push_back(neigh);
        if(parseTree.information.find(neigh) != parseTree.information.end() and
        parseTree.information[neigh]->getType() == EntityType::SIMPLIFIEDOperator) {
            if(operators.whichOperator(0, parseTree.information[neigh]->getString()) == targetOperator) {
                if(!andInside) {
                    andInside = true;
                    if(pile.size() < 2) {
                        throw logic_error("Expected predicate before AND on the pile");
                    }
                    begin = (int)pile.size() - 2;
                }
            } else if(any_of(lowerOperators.begin(), lowerOperators.end(), [&](const std::string& current) {
                          return current == operators.whichOperator(0, parseTree.information[neigh]->getString());
                      })) {
                if(andInside) {
                    auto newNode = parseTree.getNextNode();
                    for(int ind = begin; ind < (int)pile.size() - 2; ++ind) {
                        parseTree.graph[newNode].push_back(pile[ind]);
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
    wasModified |= (parseTree.graph[node] != pile);
    parseTree.graph[node] = pile;
    pile.clear();
    for(auto& neigh : parseTree.graph[node]) {
        wasModified |= applyParanthesesToOperators(neigh, targetOperator, lowerOperators);
    }
    return wasModified;
}

bool Reducer::applyParanthesesToConjunctions(int node) {
    return applyParanthesesToOperators(node, "AND", { "OR", "IMPLY", "DOUBLEImply" });
}

bool Reducer::applyParanthesesToDisjunctions(int node) {
    return applyParanthesesToOperators(node, "OR", { "IMPLY", "DOUBLEImply" });
}

bool Reducer::applyParanthesesToImplications(int node) {
    return applyParanthesesToOperators(node, "IMPLY", { "DOUBLEImply" });
}

bool Reducer::eliminateDoubleImplicationOrImplication(bool isDoubleImplication, int node) {
    static vector<int> pile;
    Operators& operators = Operators::getInstance();
    pile.clear();
    if(!isDoubleImplication) {
        /// that's because it's implication and this is
        /// RIGHT ASSOCIATIVE
        reverse(parseTree.graph[node].begin(), parseTree.graph[node].end());
    }
    for(auto& neigh : parseTree.graph[node]) {
        pile.push_back(neigh);
        if(pile.size() >= 3 and parseTree.information.find(pile[(int)pile.size() - 2]) != parseTree.information.end()) {
            auto whichOperator = operators.whichOperator(0, parseTree.information[pile[(int)pile.size() - 2]]->getString());
            if(isDoubleImplication and whichOperator == "DOUBLEImply") {
                auto rightPredicate = pile.back();
                pile.pop_back();
                auto doubleImply = pile.back();
                pile.pop_back();
                auto leftPredicate = pile.back();
                pile.pop_back();
                disposeNode(doubleImply);
                pile.push_back(addImplication(leftPredicate, rightPredicate));
                pile.push_back(addNodeWithOperator("AND"));
                pile.push_back(addImplication(rightPredicate, leftPredicate));
            } else if(!isDoubleImplication and whichOperator == "IMPLY") {
                auto leftPredicate = pile.back();
                pile.pop_back();
                auto imply = pile.back();
                pile.pop_back();
                auto rightPredicate = pile.back();
                pile.pop_back();
                disposeNode(imply);
                auto negatedLeftPredicate = addNegationToFormula(leftPredicate);
                pile.push_back(addOrClause(negatedLeftPredicate, rightPredicate));
            }
        }
    }
    bool wasModified = false;
    wasModified |= (parseTree.graph[node] != pile);
    if(!isDoubleImplication) {
        // this is because we have to revert the correct order for implication
        reverse(parseTree.graph[node].begin(), parseTree.graph[node].end());
        reverse(pile.begin(), pile.end());
    }
    parseTree.graph[node] = pile;
    pile.clear();
    for(auto& neigh : parseTree.graph[node]) {
        wasModified |= eliminateDoubleImplicationOrImplication(isDoubleImplication, neigh);
    }
    return wasModified;
}

// warm-up for step 1.1)
bool Reducer::reduceDoubleImplicationStep(int node) {
    bool isDone = true;
    cerr << "beginning " << parseTree.getEulerTraversal() << '\n';
    if(applyParanthesesToConjunctions(node)) {
        if(applyParanthesesToConjunctions(node)) {
            throw logic_error("it should not get modified twice when "
                              "applying applyParanthesesToConjunctions");
        }
        isDone = false;
    }
    cerr << "after parantheses to conjunctions " << parseTree.getEulerTraversal() << '\n';
    if(applyParanthesesToDisjunctions(node)) {
        if(applyParanthesesToDisjunctions(node)) {
            throw logic_error("it should not get modified twice when "
                              "applying applyParanthesesToDisjunctions");
        }
        isDone = false;
    }
    cerr << "after parantheses to disjunctions " << parseTree.getEulerTraversal() << '\n';
    if(applyParanthesesToImplications(node)) {
        if(applyParanthesesToImplications(node)) {
            throw logic_error("it should not get modified twice when "
                              "applying applyParanthesesToImplications");
        }
        isDone = false;
    }
    cerr << "after parantheses to implications " << parseTree.getEulerTraversal() << '\n';
    if(eliminateDoubleImplicationOrImplication(true, node)) {
        if(eliminateDoubleImplicationOrImplication(true, node)) {
            throw logic_error("it should not get modified twice when "
                              "applying eliminateDoubleImplication");
        }
        isDone = false;
    }
    cerr << "after parantheses to double implications " << parseTree.getEulerTraversal() << '\n';
    return isDone;
}

bool Reducer::resolveRightAssociativityForImplications(int node) {
    return eliminateDoubleImplicationOrImplication(false, node);
}

// does step 1.1)
bool Reducer::reduceImplicationStep(int node) {
    bool wasModified = false;
    while(!reduceDoubleImplicationStep(node)) { wasModified = true; }
    if(resolveRightAssociativityForImplications(node)) {
        cerr << "after removing implications " << parseTree.getEulerTraversal() << '\n';
        if(resolveRightAssociativityForImplications(node)) {
            throw logic_error("it should not get modified twice when "
                              "applying resolveRightAssociativityForImplications");
        }
        wasModified = true;
    }
    return wasModified;
}

// does step 1.2) - 1.6)
bool Reducer::pushNOTStep(int node) {
    Operators& operators = Operators::getInstance();
    bool isNot           = false;
    if(parseTree.information.find(node) != parseTree.information.end()) {
        isNot = operators.isNot(parseTree.information[node]->getString());
    }
    string operatorOnTheLevel;
    for(auto& neighbour : parseTree.graph[node]) {
        if(parseTree.information.find(node) != parseTree.information.end()) {
            if(parseTree.information[node]->getType() == EntityType::SIMPLIFIEDOperator) {
                if(operatorOnTheLevel.empty()) {
                    operatorOnTheLevel = parseTree.information[node]->getString();
                    if(operatorOnTheLevel == operators.NOT) {
                        // that's not unary
                        // then reset
                        operatorOnTheLevel.clear();
                    } else if(operatorOnTheLevel != operators.AND and operatorOnTheLevel != operators.OR) {
                        // at this point the tree should have only AND, OR and NOT + quantifiers
                        throw logic_error("at this point the tree should have only AND, OR and NOT + quantifiers");
                    }
                } else {
                    if(operatorOnTheLevel != parseTree.information[node]->getString()) {
                        throw logic_error("all of the simplified operators which are on"
                                          "the same level should be the same at this point");
                    }
                }
            }
        }
    }
    bool wasModified = false;
    if(isNot) {
        vector<int> newNodes;
        /// at this point we know that all what we have is either a CNF or a DNF
        for(auto& neighbour : parseTree.graph[node]) {
            newNodes.push_back(neighbour);
            if(parseTree.information.find(neighbour) != parseTree.information.end()) {
                switch(parseTree.information[neighbour]->getType()) {
                case BOUNDVariable: {
                    newNodes.pop_back();
                    if(operators.isQuantifierAndVariable(parseTree.information[neighbour]->getString())) {
                        auto newEntity =
                        getEntityWithFlippedQuantifierAndVariable(parseTree.information[neighbour]->getString());
                        parseTree.information[neighbour] = parseTree.information[node];
                        auto newNode                     = parseTree.getNextNode();
                        parseTree.information[newNode]   = newEntity;
                        parseTree.graph[newNode].push_back(neighbour);
                        newNodes.push_back(newNode);
                    } else {
                        throw logic_error("BOUNDVariable should always contain a quantifier "
                                          "and a variable but it was given the "
                                          "following " +
                        parseTree.information[neighbour]->getString());
                    }
                    break;
                }
                case SIMPLIFIEDOperator: {
                    if(operators.whichOperator(0, parseTree.information[neighbour]->getString()) == "NOT") {
                        parseTree.information.erase(parseTree.information.find(neighbour));
                    } else if(operators.isAnd(parseTree.information[neighbour]->getString()) or
                    operators.isOr(parseTree.information[neighbour]->getString())) {
                        auto target = operators.flipAndOrOr(parseTree.information[neighbour]->getString());
                        parseTree.information[neighbour] = make_shared<Entity>(EntityType::SIMPLIFIEDOperator, target);
                    }
                    break;
                }
                case SIMPLIFIEDLiteral:
                    parseTree.information[neighbour]->getEntity<shared_ptr<SimplifiedLiteral>>()->negate();
                    break;
                case NORMALForms:
                default: throw logic_error("at this point the atoms are not grouped on normal forms");
                }
            } else {
                parseTree.information[neighbour] = parseTree.information[node];
            }
        }
        wasModified = true;
        parseTree.information.erase(parseTree.information.find(node));
        parseTree.graph[node] = newNodes;
    }
    for(auto& neighbour : parseTree.graph[node]) { wasModified |= pushNOTStep(neighbour); }
    return wasModified;
}

void Reducer::basicReduce() {
    bool doIt;
    do {
        doIt = false;
        // this corresponds to rules 1.1-1.6 from Leitsch
        // 1.1) break IMPLICATION on disjunctions
        doIt |= reduceImplicationStep(parseTree.Root);
        // 1.2) push NOT further on conjunctions
        // 1.3) push NOT further on disjunctions
        // 1.4) NOT NOT gets reducted
        // 1.5) NOT@xF => ?xNOTF
        // 1.6) NOT?xF => @xNOTF
        doIt |= pushNOTStep(parseTree.Root);
    } while(doIt);
}

bool Reducer::checkNonAmbiguousScope(int node, set<std::string>& variablesInCurrentStack, string* result) {
    Operators& operators = Operators::getInstance();
    string toRemove;
    if(parseTree.information.find(node) != parseTree.information.end()) {
        if(parseTree.information[node]->getType() == EntityType::BOUNDVariable) {
            auto information = parseTree.information[node]->getEntity<string>();
            auto quantifier  = operators.getQuantifierFromQuantifierAndVariable(information);
            auto variable    = operators.getVariableFromQuantifierAndVariable(information);
            if(variablesInCurrentStack.find(variable) != variablesInCurrentStack.end()) {
                *result = variable;
                return false;
            }
            variablesInCurrentStack.insert(variable);
            toRemove = variable;
        }
    }
    bool isOk = true;
    for(auto& neighbour : parseTree.graph[node]) {
        isOk &= checkNonAmbiguousScope(neighbour, variablesInCurrentStack, result);
    }
    if(!toRemove.empty()) {
        variablesInCurrentStack.erase(variablesInCurrentStack.find(toRemove));
    }
    return isOk;
}

void Reducer::variableRenaming(int node, set<std::string>& accumulator, unordered_map<std::string, std::string>& substitution) {
    string whichVariable;
    bool wasSubstitution = false;
    Operators& operators = Operators::getInstance();
    if(parseTree.information.find(node) != parseTree.information.end()) {
        if(parseTree.information[node]->getType() == EntityType::BOUNDVariable) {
            auto information = parseTree.information[node]->getEntity<string>();
            auto quantifier  = operators.getQuantifierFromQuantifierAndVariable(information);
            string variable  = operators.getVariableFromQuantifierAndVariable(information);
            if(accumulator.find(variable) != accumulator.end()) {
                // we want then to rename it
                substitution[variable] = getRandomFunctionOrConstantName();
                accumulator.insert(substitution[variable]);
                wasSubstitution = true;
                whichVariable   = variable;
            }
        } else if(parseTree.information[node]->getType() == EntityType::SIMPLIFIEDLiteral) {
            auto simplifiedLiteral = parseTree.information[node]->getEntity<shared_ptr<SimplifiedLiteral>>();
            simplifiedLiteral->simpleSubstitution(substitution);
        }
    }
    for(auto& neighbour : parseTree.graph[node]) { variableRenaming(neighbour, accumulator, substitution); }
    if(wasSubstitution) {
        substitution.erase(substitution.find(whichVariable));
    }
}

bool Reducer::skolemizationStep(int node,
std::vector<std::string>& variablesInUniversalQuantifiers,
unordered_map<string, SimplifiedLiteral::arg>& skolem) {
    bool wasModified    = false;
    bool wasEQuantifier = false;
    bool wasVQuantifier = false;
    string whichVariable;
    Operators& operators = Operators::getInstance();
    if(parseTree.information.find(node) != parseTree.information.end()) {
        if(parseTree.information[node]->getType() == EntityType::BOUNDVariable) {
            auto information = parseTree.information[node]->getEntity<string>();
            auto quantifier  = operators.getQuantifierFromQuantifierAndVariable(information);
            auto variable    = operators.getVariableFromQuantifierAndVariable(information);
            if(quantifier == operators.EQuantifier) {
                if(variablesInUniversalQuantifiers.empty()) {
                    skolem[variable] = getRandomFunctionOrConstantName();
                } else {
                    skolem[variable] = make_pair(getRandomFunctionOrConstantName(), variablesInUniversalQuantifiers);
                }
                wasModified |= true;
                wasEQuantifier = true;
                whichVariable  = variable;
            } else {
                if(quantifier != operators.VQuantifier) {
                    throw logic_error("the quantifier should be either existential or universal");
                }
                variablesInUniversalQuantifiers.push_back(variable);
                wasVQuantifier = true;
            }
        } else if(parseTree.information[node]->getType() == EntityType::SIMPLIFIEDLiteral) {
            auto simplifiedLiteral = parseTree.information[node]->getEntity<shared_ptr<SimplifiedLiteral>>();
            wasModified |= simplifiedLiteral->substituteSkolem(skolem);
        }
    }
    for(auto& neighbour : parseTree.graph[node]) {
        wasModified |= skolemizationStep(neighbour, variablesInUniversalQuantifiers, skolem);
    }
    if(wasEQuantifier) {
        skolem.erase(skolem.find(whichVariable));
        /// here we delete the information for this node
        parseTree.information.erase(parseTree.information.find(node));
    }
    if(wasVQuantifier) {
        variablesInUniversalQuantifiers.pop_back();
    }
    return wasModified;
}

void Reducer::skolemization() {
    set<std::string> variablesSoFar;
    string violatingVariable;
    if(!checkNonAmbiguousScope(parseTree.Root, variablesSoFar, &violatingVariable)) {
        throw logic_error("the given formula has ambiguities in variable namings; "
                          "the variable " +
        violatingVariable + " apears multiple times in a chain of quantifiers");
    }
    if(!variablesSoFar.empty()) {
        throw logic_error("something went wrong with checkNonAmbiguousScope function; content partially disposed");
    }
    // in order to disambiguate the formula, make the variables unique
    unordered_map<string, string> simpleSubstitution;
    variableRenaming(parseTree.Root, variablesSoFar, simpleSubstitution);
    vector<std::string> variablesInUniversalQuantifiers;
    unordered_map<string, variant<string, pair<string, vector<string>>>> skolem;
    while(skolemizationStep(parseTree.Root, variablesInUniversalQuantifiers, skolem)) {
        if(!variablesInUniversalQuantifiers.empty() or !skolem.empty()) {
            throw logic_error("skolemization does not dispose the right content between two independent executions");
        }
    }
}

unordered_set<string> Reducer::getAllVariables() {
    unordered_set<string> variables;
    Operators& operators = Operators::getInstance();
    for(auto& information : parseTree.information) {
        auto value = information.second;
        if(value->getType() == EntityType::SIMPLIFIEDLiteral) {
            // TODO: delete eventually
            // auto simplifiedLiteral = value->getEntity<shared_ptr<Literal>>();
            // auto arguments         = simplifiedLiteral->getArguments();
            // for(auto& argument : arguments) {
            //     if(argument.index() == 0) {
            //        variables.insert(get<0>(argument));
            //     }
            // }
            // here we do continue because we don't want to count the constants
            continue;
        } else if(value->getType() == EntityType::BOUNDVariable) {
            auto boundVariable = value->getEntity<string>();
            auto variable      = operators.getVariableFromQuantifierAndVariable(boundVariable);
            variables.insert(variable);
        } else if(information.first == EntityType::NORMALForms) {
            throw logic_error("At this point we don't expect any NORMALForms in the ParseTree");
        }
    }
    return variables;
}

void Reducer::removeUniversalQuantifiers() {
    vector<int> universalQuantifiersNodes;
    for(auto& information : parseTree.information) {
        auto key   = information.first;
        auto value = information.second;
        if(value->getType() == BOUNDVariable) {
            universalQuantifiersNodes.emplace_back(key);
        }
    }
    for(auto& elem : universalQuantifiersNodes) { parseTree.information.erase(parseTree.information.find(elem)); }
}

shared_ptr<SimplifiedClauseForm> Reducer::unifyTwoNormalFormsOnOperator(const shared_ptr<SimplifiedClauseForm>& first,
const shared_ptr<SimplifiedClauseForm>& second,
bool isAnd,
const std::vector<SimplifiedLiteral::arg>& arguments) {
    if(first->isEmpty) {
        return make_shared<SimplifiedClauseForm>(second->simplifiedLiterals);
    }
    if(second->isEmpty) {
        return make_shared<SimplifiedClauseForm>(first->simplifiedLiterals);
    }
    std::vector<SimplifiedClauseForm::SimplifiedClause> firstClauses(first->getSimplifiedClauseForm());
    std::vector<SimplifiedClauseForm::SimplifiedClause> secondClauses(second->getSimplifiedClauseForm());
    if(isAnd) {
        firstClauses.insert(end(firstClauses), begin(secondClauses), end(secondClauses));
        return make_shared<SimplifiedClauseForm>(firstClauses);
    }
    std::string fakePredicateName = getRandomPredicateName();
    // uncomment here if you want to escape from the optimization
    if(firstClauses.size() == 1 and secondClauses.size() == 1) {
        /// trivial case
        firstClauses[0].insert(end(firstClauses[0]), begin(secondClauses[0]), end(secondClauses[0]));
        return make_shared<SimplifiedClauseForm>(firstClauses);
    }
    shared_ptr<SimplifiedLiteral> newLiteral = make_shared<SimplifiedLiteral>(false, fakePredicateName, arguments);
    shared_ptr<SimplifiedLiteral> newLiteralNegated = make_shared<SimplifiedLiteral>(true, fakePredicateName, arguments);
    for(auto& clause : firstClauses) { clause.push_back(newLiteral); }
    for(auto& clause : secondClauses) { clause.push_back(newLiteralNegated); }
    firstClauses.insert(end(firstClauses), begin(secondClauses), end(secondClauses));
    return make_shared<SimplifiedClauseForm>(firstClauses);
}

void Reducer::unifyNormalForms(int node, const std::vector<SimplifiedLiteral::arg>& arguments) {
    int cnt = 0;
    int whichNeighbour;
    for(auto& neighbour : parseTree.graph[node]) {
        unifyNormalForms(neighbour, arguments);
        if(parseTree.information.find(neighbour) != parseTree.information.end()) {
            cnt += 1;
            whichNeighbour = neighbour;
        }
    }
    if(cnt == 1) {
        if(parseTree.information.find(node) != parseTree.information.end()) {
            throw logic_error("two instances cannot be in a relationship father-son");
        }
        swap(parseTree.information[node], parseTree.information[whichNeighbour]);
        for(auto& x : parseTree.graph[node]) { disposeNode(x); }
        parseTree.graph[node].clear();
    }
    int totalNumberOfOperators = 0;
    int totalNumberOfClauses   = 0;
    for(auto& neighbour : parseTree.graph[node]) {
        if(parseTree.information.find(neighbour) != parseTree.information.end()) {
            if(parseTree.information[neighbour]->getType() == EntityType::SIMPLIFIEDOperator) {
                totalNumberOfOperators += 1;
            } else if(parseTree.information[neighbour]->getType() == EntityType::NORMALForms) {
                totalNumberOfClauses += 1;
            }
        }
    }
    Operators& operators = Operators::getInstance();
    if(parseTree.graph[node].empty() and parseTree.information.find(node) != parseTree.information.end() and
    parseTree.information[node]->getType() == EntityType::SIMPLIFIEDLiteral) {
        /// simple leaf which is simplifiedLiteral
        vector<SimplifiedClauseForm::SimplifiedClause> clauses;
        clauses.push_back({ parseTree.information[node]->getEntity<shared_ptr<SimplifiedLiteral>>() });
        shared_ptr<SimplifiedClauseForm> normalForm = make_shared<SimplifiedClauseForm>(clauses);
        parseTree.information[node]                 = make_shared<Entity>(EntityType::NORMALForms, normalForm);
    } else if(totalNumberOfClauses and totalNumberOfOperators) {
        // we are in a node and that one is having the sons forming a normal form
        parseTree.information[node] = make_shared<Entity>(EntityType::NORMALForms, make_shared<SimplifiedClauseForm>());
        bool isAnd                  = false;
        for(auto& neighbour : parseTree.graph[node]) {
            if(parseTree.information.find(neighbour) == parseTree.information.end()) {
                continue;
            } else {
                if(parseTree.information[neighbour]->getType() == EntityType::SIMPLIFIEDLiteral) {
                    throw logic_error("at this point it should be no simplifiedLiteral in the parse tree");
                } else if(parseTree.information[neighbour]->getType() == EntityType::NORMALForms) {
                    parseTree.information[node] = make_shared<Entity>(EntityType::NORMALForms,
                    unifyTwoNormalFormsOnOperator(parseTree.information[node]->getEntity<shared_ptr<SimplifiedClauseForm>>(),
                    parseTree.information[neighbour]->getEntity<shared_ptr<SimplifiedClauseForm>>(), isAnd, arguments));
                } else if(parseTree.information[neighbour]->getType() == EntityType::SIMPLIFIEDOperator) {
                    isAnd = operators.isAnd(parseTree.information[neighbour]->getEntity<string>());
                }
            }
        }
    }
}

template <typename T> T getSimplifiedClauseForm() {
    throw logic_error("not implemented");
}

template <> std::vector<SimplifiedClauseForm::SimplifiedClause> Reducer::getSimplifiedClauseForm() {
    static bool executed = false;
    static std::vector<SimplifiedClauseForm::SimplifiedClause> clauseForm;
    if(!executed) {
        cerr << "before everything : " << parseTree.getEulerTraversal() << endl;
        basicReduce();
        cerr << "after basic reduction : " << parseTree.getEulerTraversal() << endl;
        skolemization();
        cerr << "after skolemization : " << parseTree.getEulerTraversal() << endl;
        auto allVariables = getAllVariables();
        std::vector<SimplifiedLiteral::arg> arguments;
        arguments.reserve(allVariables.size());
        for(auto& variable : allVariables) { arguments.emplace_back(variable); }
        if(arguments.empty()) {
            // we'll introduce a constant here, in order to do not allow predicates of arity 0
            arguments.emplace_back(getRandomFunctionOrConstantName());
        }
        cerr << parseTree.getEulerTraversal() << endl;
        removeUniversalQuantifiers();
        unifyNormalForms(parseTree.Root, arguments);
        clauseForm =
        parseTree.information[parseTree.Root]->getEntity<shared_ptr<SimplifiedClauseForm>>()->getSimplifiedClauseForm();
        executed = true;
        cerr << parseTree.getEulerTraversal() << endl;
    }
    return clauseForm;
}

template <> string Reducer::getSimplifiedClauseForm() {
    std::vector<SimplifiedClauseForm::SimplifiedClause> clauseForm =
    getSimplifiedClauseForm<std::vector<SimplifiedClauseForm::SimplifiedClause>>();
    string result = "{";
    for(int ind = 0; ind < (int)clauseForm.size(); ++ind) {
        auto clause = clauseForm[ind];
        result += SimplifiedClauseForm::getString(clause);
        if(ind + 1 < (int)clauseForm.size()) {
            result += ", ";
        }
    }
    result += "}";
    return result;
}
} // namespace utils
