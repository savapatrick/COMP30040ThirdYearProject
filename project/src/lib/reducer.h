//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_REDUCER_H
#define PROJECT_REDUCER_H

#include "clause_form.h"
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
    bool computedClauseForm;
    std::unordered_set<std::string> allBoundVariables;
    std::unordered_set<std::string> reservedTermNames;
    std::unordered_set<std::string> reservedFunctionNames;
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

    void variableRenaming(int node, std::unordered_set<std::string>& accumulator, std::unordered_map<std::string, std::string>& substitution);
    void constantRenaming(int node,
    std::unordered_set<std::string>& variablesInQuantifiers,
    std::unordered_map<std::string, std::string>& substitution);

    void disambiguateFormula();

    bool skolemizationStep(int node,
    std::vector<std::string>& variablesInUniversalQuantifiers,
    std::unordered_map<std::string, SimplifiedLiteral::arg>& skolem);

    static std::shared_ptr<Entity> getEntityWithFlippedQuantifierAndVariable(const std::string& which);

    std::string getRandomTermName();
    std::string getRandomFunctionName();
    std::string getRandomPredicateName();

    void basicReduce();

    void skolemization();

    void removeUniversalQuantifiers();

    std::shared_ptr<SimplifiedClauseForm> unifyTwoNormalFormsOnOperator(const std::shared_ptr<SimplifiedClauseForm>& first,
    const std::shared_ptr<SimplifiedClauseForm>& second,
    bool isAnd);
    void unifyNormalForms(int node);

    public:
    explicit Reducer(ParseTree& _parseTree);

    void addNegationToRoot();
    template <typename T> T getSimplifiedClauseForm();
    std::shared_ptr<ClauseForm> getClauseForm();
};
}; // namespace utils

#endif // PROJECT_REDUCER_H
