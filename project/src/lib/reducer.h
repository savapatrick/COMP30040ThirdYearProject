//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_REDUCER_H
#define PROJECT_REDUCER_H

#include "entity.h"
#include "parse_tree.h"
#include <map>
#include <set>

namespace utils {
class ParseTree;

class Reducer {
    private:
    ParseTree& parseTree;
    std::set<std::string> reservedVariableNames;

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

    bool skolemizationStep(int node,
    std::set<std::string>& variablesSoFar,
    std::vector<std::string>& variablesInUniversalQuantifiers,
    std::map<std::string, Literal::arg>& skolem);

    bool convertToCNFStep(int node);

    std::string extractClauseForm();

    Entity mergeSameNormalFormEntities(const Entity& first, const Entity& second);

    static std::shared_ptr<Entity> getEntityWithFlippedQuantifierAndVariable(const std::string& which);

    std::string getRandomFunctionOrConstantName();

    public:
    explicit Reducer(ParseTree& _parseTree);

    void basicReduce();

    void skolemization();

    void convertToCNF();

    std::string getClauseForm();
};
}; // namespace utils

#endif // PROJECT_REDUCER_H
