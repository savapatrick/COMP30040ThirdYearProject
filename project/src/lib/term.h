//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_TERM_H
#define PROJECT_TERM_H

#include <string>
#include <vector>
namespace utils {
class Term {
    private:
    public:
    explicit Term(const std::string& term);
    explicit Term(const std::pair<std::string, std::vector<std::string>>& function);
};
}; // namespace utils

#endif // PROJECT_TERM_H
