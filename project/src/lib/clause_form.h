//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_CLAUSE_FORM_H
#define PROJECT_CLAUSE_FORM_H

#include "simplified_clause_form.h"
namespace utils {
class ClauseForm {
    private:
    public:
    ClauseForm(const std::vector<SimplifiedClauseForm::SimplifiedClause>& simplifiedClauseForm,
    const std::unordered_set <std::string>& functionNames,
    const std::unordered_set <std::string>& variableNames,
    const std::unordered_set <std::string>& constantNames) {
    }
};
}; // namespace utils

#endif // PROJECT_CLAUSE_FORM_H
