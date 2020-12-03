//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_CLAUSE_H
#define PROJECT_CLAUSE_H

#include "literal.h"
#include "simplified_clause_form.h"

namespace utils {

class Clause {
    private:
    std::vector <std::shared_ptr<Literal>> clause;
    public:
    Clause(const SimplifiedClauseForm::SimplifiedClause& simplifiedCause,
           const std::unordered_set<std::string>& variableNames,
           const std::unordered_set<std::string>& constantNames) {
        clause.reserve(simplifiedCause.size());
        for (auto &simplifiedLiteral : simplifiedCause) {
            clause.push_back(std::make_shared<Literal>(simplifiedLiteral, variableNames, constantNames));
        }
    }
};
}; // namespace utils

#endif // PROJECT_CLAUSE_H
