//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_CLAUSE_FORM_H
#define PROJECT_CLAUSE_FORM_H

#include "clause.h"
#include "simplified_clause_form.h"

namespace utils {
class BasicTheoremProver;
class ClauseForm {
    private:
    friend class BasicTheoremProver;
    std::vector<std::shared_ptr<Clause>> clauseForm;
    std::unordered_set<std::string> allFunctionNames;
    std::unordered_set<std::string> allVariableNames;
    std::unordered_set<std::string> allConstantNames;

    public:
    ClauseForm(const std::shared_ptr<SimplifiedClauseForm>& simplifiedClauseForm,
    const std::unordered_set<std::string>& functionNames,
    const std::unordered_set<std::string>& variableNames,
    const std::unordered_set<std::string>& constantNames) {
        allFunctionNames                  = functionNames;
        allVariableNames                  = variableNames;
        allConstantNames                  = constantNames;
        auto deepCopy                     = simplifiedClauseForm->clone();
        allVariableNames                  = deepCopy->makeVariableNamesUniquePerClause(allVariableNames);
        auto simplifiedClauseFormDeepCopy = deepCopy->getSimplifiedClauseForm();
        clauseForm.reserve(simplifiedClauseFormDeepCopy.size());
        for(auto& simplifiedClause : simplifiedClauseFormDeepCopy) {
            clauseForm.push_back(std::make_shared<Clause>(simplifiedClause, variableNames, constantNames));
        }
    }
    void merge(std::shared_ptr<ClauseForm>& other);
    void applySubstitution(const std::pair<std::string, std::string>& mapping);
    void renameFunction(const std::pair<std::string, std::string>& mapping);
    void renameTerms(std::shared_ptr<ClauseForm>& other,
    std::unordered_set<std::string>& _allTermNames,
    std::unordered_set<std::string>& _allTermNamesOther);
    void makeVariableNamesUniquePerClause();
};
}; // namespace utils

#endif // PROJECT_CLAUSE_FORM_H
