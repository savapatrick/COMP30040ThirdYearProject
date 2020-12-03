//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_CLAUSE_FORM_H
#define PROJECT_CLAUSE_FORM_H

#include "simplified_clause_form.h"
#include "clause.h"

namespace utils {
class ClauseForm {
    private:
    std::vector <std::shared_ptr<Clause>> clauseForm;
    std::unordered_set<std::string> allFunctionNames; // to do not be used
    std::unordered_set<std::string> allVariableNames;
    std::unordered_set<std::string> allConstantNames;
    std::vector<SimplifiedClauseForm::SimplifiedClause> makeVariableNamesUniquePerClause(const std::shared_ptr<SimplifiedClauseForm>& simplifiedClauseForm);
    public:
    ClauseForm(const std::shared_ptr<SimplifiedClauseForm>& simplifiedClauseForm,
    const std::unordered_set<std::string>& functionNames,
    const std::unordered_set<std::string>& variableNames,
    const std::unordered_set<std::string>& constantNames) {
        allFunctionNames = functionNames;
        allVariableNames = variableNames;
        allConstantNames = constantNames;
        auto deepCopy = simplifiedClauseForm->clone();
    }
};
}; // namespace utils

#endif // PROJECT_CLAUSE_FORM_H
