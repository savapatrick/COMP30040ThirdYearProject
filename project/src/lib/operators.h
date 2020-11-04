//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_OPERATORS_H
#define PROJECT_OPERATORS_H

#include <string>

namespace utils {
    // singleton class for tokens
    class Tokens {
    public:
        const std::string AND;
        const std::string OR;
        const std::string NOT;
        const std::string OPENEDBracket;
        const std::string CLOSEDBracket;
        const std::string IMPLY;
        const std::string EQuantifier;
        const std::string VQuantifier;
    private:
        Tokens(): AND("&"), OR("|"), NOT("~"), OPENEDBracket("("),
        CLOSEDBracket(")"), IMPLY("->"), EQuantifier("?"), VQuantifier("@"){}
        Tokens(Tokens const&);
        void operator=(Tokens const&);
    public:
        static Tokens &getInstance() {
            static Tokens instance;
            return instance;
        }
    };
}

#endif //PROJECT_OPERATORS_H
