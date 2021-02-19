//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_TOKENIZER_H
#define PROJECT_TOKENIZER_H

#include <string>
#include <variant>
#include <vector>

namespace utils {
class Tokenizer {
    private:
    Tokenizer(){};

    Tokenizer(Tokenizer const&);

    void operator=(Tokenizer const&);

    public:
    static Tokenizer& getInstance() {
        static Tokenizer instance;
        return instance;
    }

    [[nodiscard]] static std::vector<std::string> tokenize(const std::string& seq) ;

    static std::pair<std::string, std::vector<std::variant<std::string, std::pair<std::string, std::vector<std::string>>>>>
    decomposePredicate(const std::string& seq);
};
}; // namespace utils

#endif // PROJECT_TOKENIZER_H
