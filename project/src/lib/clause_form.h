//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_CLAUSE_FORM_H
#define PROJECT_CLAUSE_FORM_H

#include "clause.h"
#include "simplified_clause_form.h"

namespace utils {
class BasicTheoremProver;
class TwoVariableTheoremProver;
class Unification;
class ClauseForm {
    private:
    friend class BasicTheoremProver;
    friend class TwoVariableTheoremProver;
    friend class Unification;
    std::vector<std::shared_ptr<Clause>> clauseForm;
    std::unordered_set<std::string> allFunctionNames;
    std::unordered_set<std::string> allVariableNames;
    std::unordered_set<std::string> allConstantNames;

    public:
    ClauseForm() = default;
    ClauseForm(const std::shared_ptr<SimplifiedClauseForm>& simplifiedClauseForm,
    const std::unordered_set<std::string>& functionNames,
    const std::unordered_set<std::string>& variableNames,
    const std::unordered_set<std::string>& constantNames) {
        allFunctionNames       = functionNames;
        allVariableNames       = variableNames;
        allConstantNames       = constantNames;
        auto simplifiedClauses = simplifiedClauseForm->getSimplifiedClauseForm();
        clauseForm.reserve(simplifiedClauses.size());
        for(auto& simplifiedClause : simplifiedClauses) {
            clauseForm.push_back(std::make_shared<Clause>(simplifiedClause, variableNames, constantNames));
        }
        makeVariableNamesUniquePerClause();
    }
    ClauseForm(const std::shared_ptr<ClauseForm>& other) {
        clauseForm.reserve(other->clauseForm.size());
        for(auto& clause : other->clauseForm) { clauseForm.push_back(clause->createDeepCopy()); }
        allFunctionNames = other->allFunctionNames;
        allVariableNames = other->allVariableNames;
        allConstantNames = other->allConstantNames;
        makeVariableNamesUniquePerClause();
    }
    void merge(std::shared_ptr<ClauseForm>& other);
    void applySubstitution(const std::pair<std::string, std::string>& mapping);
    void renameFunction(const std::pair<std::string, std::string>& mapping);
    void renameTerms(std::shared_ptr<ClauseForm>& other,
    std::unordered_set<std::string>& _allTermNames,
    std::unordered_set<std::string>& _allTermNamesOther,
    std::unordered_set<std::string>& forbiddenOne,
    std::unordered_set<std::string>& forbiddenTwo,
    bool isFunctionRenaming);
    void makeVariableNamesUniquePerClause();
    std::string getString() const;
    std::string getStringWithIndex() const;
    std::string getStringWithIndex(const std::unordered_map<int, int>& isDeleted) const;
    bool isTwoVariableFragment();
};
}; // namespace utils

#endif // PROJECT_CLAUSE_FORM_H
