//
// Created by Patrick Sava on 11/18/2020.
//

#ifndef PROJECT_SIMPLIFIED_CLAUSE_FORM_H
#define PROJECT_SIMPLIFIED_CLAUSE_FORM_H

#include "simplified_literal.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace utils {

class SimplifiedClauseForm {
    public:
    typedef std::vector<std::shared_ptr<SimplifiedLiteral>> SimplifiedClause;

    private:
    friend class Reducer;

    private:
    bool isEmpty;
    std::vector<SimplifiedClause> simplifiedLiterals;

    public:
    [[nodiscard]] const std::vector<SimplifiedClause>& getSimplifiedClauseForm() const;

    public:
    SimplifiedClauseForm() : isEmpty(true) {
        simplifiedLiterals.clear();
    }
    SimplifiedClauseForm(std::vector<SimplifiedClause> _simplifiedLiterals) {
        isEmpty            = _simplifiedLiterals.empty();
        simplifiedLiterals = std::move(_simplifiedLiterals);
    }
    [[nodiscard]] static std::string getString(const SimplifiedClause& clause);
    [[nodiscard]] std::string getString() const;
};
}; // namespace utils

#endif // PROJECT_SIMPLIFIED_CLAUSE_FORM_H
