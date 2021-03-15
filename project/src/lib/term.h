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
    std::string termName;
    TermType termType; // function name, constant name, variable name
    std::vector<std::shared_ptr<Term>> arguments;
    std::string cachedGetString;
    std::string cachedGetStringWithoutVariableNames;
    std::string cachedGetHash;
    bool containsTerm(const std::string& name) const;
    std::variant<bool, std::pair<std::string, std::shared_ptr<Term>>>
    findPartialSubstitution(const std::shared_ptr<Term>& first, const std::shared_ptr<Term>& second) const;
    std::string preOrderTraversal(const std::shared_ptr<const Term>& node,
    const std::unordered_map<std::string, std::string>& substitution) const;
    void getDepths(const std::shared_ptr<const Term>& node, std::unordered_map<std::string, int>& soFar, int currentDepth, int& maxDepth) const;

    public:
    explicit Term(std::string newVariable) : termName(std::move(newVariable)), termType(VARIABLE) {
        arguments.clear();
        cachedGetString.clear();
        cachedGetStringWithoutVariableNames.clear();
        cachedGetHash.clear();
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
        cachedGetString.clear();
        cachedGetStringWithoutVariableNames.clear();
        cachedGetHash.clear();
    }
    explicit Term(const std::pair<std::string, std::vector<std::string>>& function,
    const std::unordered_set<std::string>& variableNames,
    const std::unordered_set<std::string>& constantNames)
    : termName(function.first), termType(FUNCTION) {
        arguments.reserve(function.second.size());
        for(auto& arg : function.second) {
            arguments.push_back(std::make_shared<Term>(arg, variableNames, constantNames));
        }
        cachedGetString.clear();
        cachedGetStringWithoutVariableNames.clear();
        cachedGetHash.clear();
    }
    explicit Term(const std::shared_ptr<Term>& other) : termName(other->termName), termType(other->termType) {
        arguments.reserve(other->arguments.size());
        auto& otherArguments = other->arguments;
        for(auto& arg : otherArguments) { arguments.push_back(arg->createDeepCopy()); }
        cachedGetString.clear();
        cachedGetStringWithoutVariableNames.clear();
        cachedGetHash.clear();
    }
    const std::string& getTermName() const;
    TermType getTermType() const;
    const std::vector<std::shared_ptr<Term>>& getArguments() const;
    std::unordered_set<std::string> getAllVariables() const;
    std::vector<std::string> getAllVariablesInOrder() const;
    bool hasNestedFunctions() const;
    std::pair<int, std::unordered_map<std::string, int>> getDepths() const;

    public: // non-const ones
    [[nodiscard]] bool equals(const std::shared_ptr<Term>& other);
    std::shared_ptr<Term> createDeepCopy();
    void applySubstitution(const std::pair<std::string, std::shared_ptr<Term>>& substitution);
    void applySubstitution(const std::pair<std::string, std::string>& substitution);
    void renameFunction(const std::pair<std::string, std::string>& substitution);
    std::variant<bool, std::pair<std::string, std::shared_ptr<Term>>> augmentUnification(const std::shared_ptr<Term>& other);
    std::string getString();
    std::string getStringWithoutVariableNames();
    std::string getHash(const std::unordered_map<std::string, std::string>& substitution);
    void clearCache();
};
}; // namespace utils

#endif // PROJECT_TERM_H
