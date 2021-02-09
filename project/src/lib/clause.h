//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_CLAUSE_H
#define PROJECT_CLAUSE_H

#include "literal.h"
#include "simplified_clause_form.h"
#include <map>

namespace utils {
class TheoremProver;
class BasicTheoremProver;
class TwoVariableTheoremProver;
class Unification;
class Clause : public std::enable_shared_from_this<Clause> {
    private:
    friend class TheoremProver;
    friend class BasicTheoremProver;
    friend class TwoVariableTheoremProver;
    friend class Unification;
    std::vector<std::shared_ptr<Literal>> clause;

    public:
    Clause(const SimplifiedClauseForm::SimplifiedClause& simplifiedCause,
    const std::unordered_set<std::string>& variableNames,
    const std::unordered_set<std::string>& constantNames) {
        clause.reserve(simplifiedCause.size());
        for(auto& simplifiedLiteral : simplifiedCause) {
            clause.push_back(std::make_shared<Literal>(simplifiedLiteral, variableNames, constantNames));
        }
    }
    Clause(const std::shared_ptr<Clause>& other) {
        clause.reserve(other->clause.size());
        auto& otherClauses = other->clause;
        for(auto& otherClause : otherClauses) { clause.push_back(otherClause->createDeepCopy()); }
    }
    Clause(const std::shared_ptr<Literal>& literal) {
        clause.reserve(1);
        clause.push_back(literal->createDeepCopy());
    }
    std::shared_ptr<Clause> createDeepCopy();
    bool hasNestedFunctions();
    std::unordered_set<std::string> getAllVariables();
    int getHighestNumberOfVariablesPerLiteral();
    void applySubstitution(const std::pair<std::string, std::shared_ptr<Term>>& mapping);
    void applySubstitution(const std::pair<std::string, std::string>& mapping);
    void renameFunction(const std::pair<std::string, std::string>& mapping);
    std::map<std::pair<std::string, bool>, int> getLiteralsAndCount() const;
    const std::vector<std::shared_ptr<Literal>>& getLiterals() const;
    std::string getString() const;
};
}; // namespace utils

#endif // PROJECT_CLAUSE_H
