//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_REDUCER_H
#define PROJECT_REDUCER_H

#include "entity.h"
#include "parse_tree.h"
#include <unordered_set>
#include <set>
#include <unordered_map>

namespace utils {
class ParseTree;

class Reducer {
    private:
    ParseTree& parseTree;
    std::unordered_set<std::string> reservedVariableNames;

    void disposeNode(int node);

    int addNodeWithOperator(const std::string& which);

    int addImplication(const int& nodeOne, const int& nodeTwo);

    int addOrClause(const int& nodeOne, const int& nodeTwo);

    int addNegationToFormula(const int& nodeOne);

    bool applyParanthesesToConjunctions(int node);

    bool applyParanthesesToDisjunctions(int node);

    bool applyParanthesesToImplications(int node);

    bool eliminateDoubleImplicationOrImplication(bool isDoubleImplication, int node);

    bool resolveRightAssociativityForImplications(int node);

    bool applyParanthesesToOperators(int node, const std::string& targetOperator, const std::vector<std::string>& lowerOperators);

    bool reduceImplicationStep(int node);

    bool reduceDoubleImplicationStep(int node);

    bool pushNOTStep(int node);

    bool checkNonAmbiguousScope(int node, std::set<std::string>& variablesInCurrentStack, std::string* result);

    void variableRenaming(int node, std::set<std::string>& accumulator, std::unordered_map<std::string, std::string>& substitution);

    bool skolemizationStep(int node,
    std::vector<std::string>& variablesInUniversalQuantifiers,
    std::unordered_map<std::string, Literal::arg>& skolem);

    shared_ptr<ClauseForm> unifyTwoNormalForms(const shared_ptr<ClauseForm>& first, const shared_ptr<ClauseForm>& second);
    bool unifyNormalForms(shared_ptr<ClauseForm> &result, int node);

    static std::shared_ptr<Entity> getEntityWithFlippedQuantifierAndVariable(const std::string& which);

    std::string getRandomFunctionOrConstantName();

    public:
    explicit Reducer(ParseTree& _parseTree);

    void basicReduce();

    void skolemization();

    template <typename T>
    T getClauseForm();
};
}; // namespace utils

#endif // PROJECT_REDUCER_H
