//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_LITERAL_H
#define PROJECT_LITERAL_H

#include "simplified_literal.h"
#include "term.h"
#include <memory>
#include <set>
#include <unordered_map>

namespace utils {
class TheoremProver;
class BasicTheoremProver;
class DepthOrderedTheoremProver;
class TwoVariableTheoremProver;
class Unification;
class Literal : public std::enable_shared_from_this<Literal> {
    private:
    friend class TheoremProver;
    friend class BasicTheoremProver;
    friend class DepthOrderedTheoremProver;
    friend class TwoVariableTheoremProver;
    friend class Unification;
    bool isNegated;
    std::string predicateName;
    std::vector<std::shared_ptr<Term>> terms;
    bool isEquality;

    public:
    Literal(const std::shared_ptr<SimplifiedLiteral>& simplifiedLiteral,
    const std::unordered_set<std::string>& variableNames,
    const std::unordered_set<std::string>& constantNames)
    : isNegated(simplifiedLiteral->getIsNegated()), predicateName(simplifiedLiteral->getPredicateName()) {
        terms.reserve(simplifiedLiteral->getArguments().size());
        auto arguments = simplifiedLiteral->getArguments();
        for(auto& arg : arguments) {
            if(arg.index()) {
                terms.push_back(std::make_shared<Term>(std::get<1>(arg), variableNames, constantNames));
            } else {
                terms.push_back(std::make_shared<Term>(std::get<0>(arg), variableNames, constantNames));
            }
        }
        isEquality = (predicateName == "Equality");
    }
    Literal(const std::shared_ptr<Literal>& other) : isNegated(other->isNegated), predicateName(other->predicateName) {
        terms.reserve(other->terms.size());
        auto& otherTerms = other->terms;
        for(auto& otherTerm : otherTerms) { terms.push_back(otherTerm->createDeepCopy()); }
        isEquality = (predicateName == "Equality");
    }

    bool equalsWithoutSign(const std::shared_ptr<Literal>& other);
    std::variant<bool, std::pair<std::string, std::shared_ptr<Term>>> augmentUnification(const std::shared_ptr<Literal>& other);
    std::shared_ptr<Literal> createDeepCopy();
    bool hasNestedFunctions();
    std::unordered_set<std::string> getAllVariables();
    std::vector<std::string> getAllVariablesInOrder();
    void applySubstitution(const std::pair<std::string, std::shared_ptr<Term>>& mapping);
    void applySubstitution(const std::pair<std::string, std::string>& mapping);
    void renameFunction(const std::pair<std::string, std::string>& mapping);
    std::pair<std::string, bool> getLiteral();
    std::string getString() const;
    std::string getStringWithoutVariableNames() const;
    std::string getHash(const std::unordered_map<std::string, std::string>& substitution) const;
    std::pair<int, std::unordered_map<std::string, int>> getDepths();
};

}; // namespace utils

#endif // PROJECT_LITERAL_H
