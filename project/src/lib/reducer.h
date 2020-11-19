//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_REDUCER_H
#define PROJECT_REDUCER_H

#include "entity.h"
#include "parse_tree.h"
#include <set>
#include <unordered_map>
#include <unordered_set>

namespace utils {
class ParseTree;

class Reducer {
    private:
    ParseTree& parseTree;
    std::unordered_set<std::string> reservedVariableNames;
    std::unordered_set<std::string> reservedPredicateNames;

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

    static std::shared_ptr<Entity> getEntityWithFlippedQuantifierAndVariable(const std::string& which);

    std::string getRandomFunctionOrConstantName();
    std::string getRandomPredicateName();

    void basicReduce();

    void skolemization();

    std::unordered_set<std::string> countVariablesAndConstants();

    void removeUniversalQuantifiers();

    shared_ptr<ClauseForm> unifyTwoNormalFormsOnOperator(const shared_ptr<ClauseForm>& first,
                                                         const shared_ptr<ClauseForm>& second,
                                                         bool isAnd,
                                                         const std::vector<Literal::arg>& arguments);
    bool unifyNormalForms(shared_ptr<ClauseForm>& result, int node, const std::vector<Literal::arg>& arguments);

    public:
    explicit Reducer(ParseTree& _parseTree);

    template <typename T> T getClauseForm();
};
}; // namespace utils

#endif // PROJECT_REDUCER_H
