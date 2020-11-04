//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_VERIFIER_H
#define PROJECT_VERIFIER_H

#include <string>

namespace utils {
    class Verifier {
    public:
        Verifier(){}
        // function which matches if the sequence is balanced
        // parantheses-wise
        static bool isBalanced(const std::string &seq);

    };
};

#endif //PROJECT_VERIFIER_H
