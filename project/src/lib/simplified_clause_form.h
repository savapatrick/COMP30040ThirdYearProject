//
// Created by Patrick Sava on 11/18/2020.
//

#ifndef PROJECT_SIMPLIFIED_CLAUSE_FORM_H
#define PROJECT_SIMPLIFIED_CLAUSE_FORM_H

#include "simplified_literal.h"
#include <memory>
#include <string>
#include <unordered_set>
#include <utility>
#include <vector>

namespace utils {

class SimplifiedClauseForm {
    public:
    typedef std::vector<std::shared_ptr<SimplifiedLiteral>> SimplifiedClause;

    private:
    friend class Reducer;

    bool isEmpty;
    std::vector<SimplifiedClause> simplifiedClauseForm;
    std::unordered_set<std::string> allArguments;

    public:
    SimplifiedClauseForm() : isEmpty(true) {
        simplifiedClauseForm.clear();
        allArguments.clear();
    }
    SimplifiedClauseForm(std::vector<SimplifiedClause> _simplifiedClauseForm) {
        isEmpty              = _simplifiedClauseForm.empty();
        simplifiedClauseForm = std::move(_simplifiedClauseForm);
        for(auto& simplifiedClause : simplifiedClauseForm) {
            for(auto& simplifiedLiteral : simplifiedClause) {
                auto args = simplifiedLiteral->getAllVariablesAndConstants();
                allArguments.insert(args.begin(), args.end());
            }
        }
        //        makeVariableNamesUniquePerClause(allArguments);
    }
    std::unordered_set<std::string> makeVariableNamesUniquePerClause(const std::unordered_set<std::string>& variables);
    std::shared_ptr<SimplifiedClauseForm> clone() const;
    [[nodiscard]] static std::string getString(const SimplifiedClause& clause);
    [[nodiscard]] std::string getString() const;
    [[nodiscard]] std::unordered_set<std::string> getAllArguments() const;
    [[nodiscard]] const std::vector<SimplifiedClause>& getSimplifiedClauseForm() const;
};
}; // namespace utils

#endif // PROJECT_SIMPLIFIED_CLAUSE_FORM_H
