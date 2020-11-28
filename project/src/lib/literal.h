//
// Created by Patrick Sava on 11/5/2020.
//

#ifndef PROJECT_LITERAL_H
#define PROJECT_LITERAL_H

#include <string>
#include <unordered_map>
#include <utility>
#include <variant>
#include <vector>

namespace utils {
class DualHashASCII;
class Literal {
    friend class DualHashASCII;

    public:
    typedef std::variant<std::string, std::pair<std::string, std::vector<std::string>>> arg;

    private:
    bool isNegated;
    std::string predicateName;
    std::vector<arg> arguments;

    public:
    Literal(bool _isNegated, std::string _predicateName, std::vector<arg> _arguments)
    : isNegated(_isNegated), predicateName(std::move(_predicateName)), arguments(std::move(_arguments)) {
    }

    static bool isLiteral(const std::string& seq);

    [[nodiscard]] bool getIsNegated() const;

    [[nodiscard]] const std::string& getPredicateName() const;

    [[nodiscard]] const std::vector<arg>& getArguments() const;

    [[nodiscard]] static std::string getArgumentString(const arg& argument);

    [[nodiscard]] std::string getString() const;

    void negate();

    void setArguments(const std::vector<arg>& args);

    bool substituteSkolem(std::unordered_map<std::string, arg>& skolem);
    void simpleSubstitution(std::unordered_map<std::string, std::string>& substitution);
};
}; // namespace utils

#endif // PROJECT_LITERAL_H
