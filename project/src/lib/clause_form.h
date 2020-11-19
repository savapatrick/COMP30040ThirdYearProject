//
// Created by Patrick Sava on 11/18/2020.
//

#ifndef PROJECT_CLAUSE_FORM_H
#define PROJECT_CLAUSE_FORM_H

#include "literal.h"
#include <memory>
#include <string>
#include <utility>
#include <vector>

namespace utils {

class ClauseForm {
    public:
    typedef std::vector<std::shared_ptr<Literal>> Clause;
    private:
    friend class Reducer;
    private:
    bool isEmpty;
    std::vector<Clause> literals;

    public:
    [[nodiscard]] const std::vector<Clause>& getClauseForm() const;

    public:
    ClauseForm() : isEmpty(true) {
        literals.clear();
    }
    ClauseForm(std::vector<Clause>  _literals)
    {
        isEmpty = _literals.empty();
        literals = std::move(_literals);
    }
    [[nodiscard]] static std::string getString(const Clause& clause) ;
    [[nodiscard]] std::string getString() const;
};
}; // namespace utils

#endif // PROJECT_CLAUSE_FORM_H
