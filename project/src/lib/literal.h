//
// Created by Patrick Sava on 11/5/2020.
//

#ifndef PROJECT_LITERAL_H
#define PROJECT_LITERAL_H

#include <map>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace utils {
class Literal {
    private:
    bool isNegated;
    std::string predicateName;
    std::vector<std::variant<std::string, std::pair<std::string, std::vector<std::string>>>> arguments;

    public:
    Literal (bool _isNegated,
    std::string _predicateName,
    std::vector<std::variant<std::string, std::pair<std::string, std::vector<std::string>>>> _arguments)
    : isNegated (_isNegated), predicateName (std::move (_predicateName)), arguments (std::move (_arguments)) {
    }

    static bool isLiteral (const std::string& seq);

    [[nodiscard]] const bool getIsNegated () const;

    [[nodiscard]] const std::string& getPredicateName () const;

    [[nodiscard]] const std::vector<std::variant<std::string, std::pair<std::string, std::vector<std::string>>>>& getArguments () const;

    [[nodiscard]] static std::string getArgumentString (
    const std::variant<std::string, std::pair<std::string, std::vector<std::string>>>& argument);

    [[nodiscard]] std::string getString () const;

    void negate ();

    void setArguments (const std::vector<std::variant<std::string, std::pair<std::string, std::vector<std::string>>>>& arguments);

    void substitute (
    const std::map<std::string, std::variant<std::string, std::pair<std::string, std::vector<std::string>>>>& skolem);
};
}; // namespace utils

#endif // PROJECT_LITERAL_H
