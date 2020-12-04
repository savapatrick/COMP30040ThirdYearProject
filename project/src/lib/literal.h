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

class Literal {
    private:
    static constexpr int CacheLIMIT() {
        return 200;
    };
    bool isNegated;
    std::string predicateName;
    std::vector<std::shared_ptr<Term>> terms;
    std::set<std::shared_ptr<Literal>> wontWork;

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

    bool equalsWithoutSign(const std::shared_ptr<Literal>& other);
    bool unify(const std::shared_ptr<Literal>& other);
};

}; // namespace utils

#endif // PROJECT_LITERAL_H
