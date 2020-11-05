//
// Created by Patrick Sava on 11/5/2020.
//

#ifndef PROJECT_ENTITY_H
#define PROJECT_ENTITY_H

#include <string>
#include <utility>
#include <vector>

namespace utils {
    class Entity{
    private:
        std::vector <std::string> terms;
    public:
        Entity() = default;
        explicit Entity(std::vector <std::string> _terms) : terms(std::move(_terms)){}
        bool isDisjunctionNormalForm();
        bool isConjunctiveNormalForm();

    };
};

#endif //PROJECT_ENTITY_H
