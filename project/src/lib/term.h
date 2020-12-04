//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_TERM_H
#define PROJECT_TERM_H

#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <variant>
#include <vector>

namespace utils {
enum TermType { FUNCTION = 0, CONSTANT, VARIABLE };
class Term : public std::enable_shared_from_this<Term> {
    private:
    std::string termName; // function name, constant name, variable name
    TermType termType;
    std::vector<std::shared_ptr<Term>> arguments;
    bool containsTerm(const std::string& name);
    std::variant<bool, std::pair <std::string, std::shared_ptr<Term>>>  findPartialSubstitution(
                          const std::shared_ptr<Term>& first,
                          const std::shared_ptr<Term>& second) const;

    public:
    explicit Term(std::string  newVariable) : termName(std::move(newVariable)), termType(VARIABLE){
        arguments.clear();
    }
    explicit Term(const std::string& term,
    const std::unordered_set<std::string>& variableNames,
    const std::unordered_set<std::string>& constantNames)
    : termName(term) {
        if(variableNames.find(term) != variableNames.end()) {
            termType = VARIABLE;
        } else {
            if(constantNames.find(term) == constantNames.end()) {
                throw std::invalid_argument("Given term " + term + " which is neither variable or constant");
            }
            termType = CONSTANT;
        }
        arguments.clear();
    }
    explicit Term(const std::pair<std::string, std::vector<std::string>>& function,
    const std::unordered_set<std::string>& variableNames,
    const std::unordered_set<std::string>& constantNames)
    : termName(function.first), termType(FUNCTION) {
        arguments.reserve(function.second.size());
        for(auto& arg : function.second) {
            arguments.push_back(std::make_shared<Term>(arg, variableNames, constantNames));
        }
    }
    explicit Term(const std::shared_ptr<Term>& other) : termName(other->termName), termType(other->termType) {
        arguments.reserve(other->arguments.size());
        auto& otherArguments = other->arguments;
        for(auto& arg : otherArguments) { arguments.push_back(std::make_shared<Term>(arg)); }
    }
    std::shared_ptr<Term> createDeepCopy();
    [[nodiscard]] bool equals(const std::shared_ptr<Term>& other) const;
    std::variant<bool, std::pair <std::string, std::shared_ptr<Term>>> augmentUnification(const std::shared_ptr<Term>& other);
    void applySubstitution(const std::pair<std::string, std::shared_ptr<Term>>& substitution);
    std::unordered_set<std::string> getAllVariables();
    std::string getString() const;
};
}; // namespace utils

#endif // PROJECT_TERM_H
