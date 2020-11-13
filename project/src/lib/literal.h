//
// Created by Patrick Sava on 11/5/2020.
//

#ifndef PROJECT_LITERAL_H
#define PROJECT_LITERAL_H

#include <string>
#include <utility>
#include <vector>

namespace utils {
    class Literal {
    private:
        bool isNegated;
        std::string predicateName;
        std::vector<std::string> arguments;
    public:
        Literal(bool _isNegated,
                std::string  _predicateName,
                std::vector<std::string>  _arguments):
        isNegated(_isNegated), predicateName(std::move(_predicateName)), arguments(std::move(_arguments)){}

        static bool isLiteral(const std::string &seq);
        [[nodiscard]] const bool getIsNegated() const;
        [[nodiscard]] const std::string &getPredicateName() const;
        [[nodiscard]] const std::vector<std::string> &getArguments() const;
        [[nodiscard]] std::string getString() const;
        void negate();
    };
};

#endif //PROJECT_LITERAL_H
