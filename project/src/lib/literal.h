//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_LITERAL_H
#define PROJECT_LITERAL_H

#include "simplified_literal.h"
#include "term.h"
#include <memory>
#include <set>

namespace utils {
class BasicTheoremProver;
class Literal : public std::enable_shared_from_this<Literal> {
    private:
    friend class BasicTheoremProver;
    bool isNegated;
    std::string predicateName;
    std::vector<std::shared_ptr<Term>> terms;

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
    }
    Literal(const std::shared_ptr<Literal>& other) : isNegated(other->isNegated), predicateName(other->predicateName) {
        terms.reserve(other->terms.size());
        auto& otherTerms = other->terms;
        for(auto& otherTerm : otherTerms) { terms.push_back(otherTerm->createDeepCopy()); }
    }

    bool equalsWithoutSign(const std::shared_ptr<Literal>& other);
    std::variant<bool, std::pair<std::string, std::shared_ptr<Term>>> augmentUnification(const std::shared_ptr<Literal>& other);
    std::shared_ptr<Literal> createDeepCopy();
    std::unordered_set<std::string> getAllVariables();
    void applySubstitution(const std::pair<std::string, std::shared_ptr<Term>>& mapping);
    void applySubstitution(const std::pair<std::string, std::string>& mapping);
    void renameFunction(const std::pair<std::string, std::string>& mapping);
    std::pair<std::string, bool> getLiteral();
    std::string getString() const;
};

}; // namespace utils

#endif // PROJECT_LITERAL_H
