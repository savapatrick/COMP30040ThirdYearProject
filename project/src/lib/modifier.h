//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_MODIFIER_H
#define PROJECT_MODIFIER_H

#include <string>
#include "operators.h"

namespace utils {
    class Modifier {
        Operators& operators;
    public:
        Modifier():operators(Operators::getInstance()){}
        /// this method will take care of the operator precendence of AND
        std::string addParantheses(const std::string& string) const;
    };
};

#endif //PROJECT_MODIFIER_H
