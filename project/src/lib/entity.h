//
// Created by Patrick Sava on 11/5/2020.
//

#ifndef PROJECT_ENTITY_H
#define PROJECT_ENTITY_H

#include <string>
#include <utility>
#include <vector>
#include <variant>
#include <stdexcept>
#include "literal.h"

namespace utils {
    enum EntityType {
        BOUNDVariable = 0, /*e.g @x, ?y, etc*/
        LITERAL, /*e.g ~P(a), ~P(f(x)), ~P(a, x, f(y))*/
        SIMPLIFIEDOperator, /*AND, OR, NOT, IMPLY, DOUBLEImply*/
        NORMALForms, /*e.g CNF or DNF, so either ~P(a) ^ B(d), or ~P(d) | P(f) could be good examples*/
    };
    enum NormalFormType{
        CNF = 0,
        DNF,
    };
    class Entity{
    public:
        typedef std::pair<NormalFormType, std::vector<Literal>> NormalFormStorage;
        typedef std::variant<std::string, /*type 0 or type 2*/
                Literal , /*type 1*/
                NormalFormStorage /*type 3*/> EntityStorage;
    private:
        EntityType type;
        EntityStorage entity;
    public:
        Entity() = default;
        explicit Entity(const EntityType &_type, EntityStorage _entity) : type(_type), entity(std::move(_entity)){
            if (!(0 <= type and type <= 3)) {
                throw std::invalid_argument("Type for Entity has to lie between 0 and 3 but given " + std::to_string(type));
            }
        };
        [[nodiscard]] EntityType getType() const;

        template<typename Value>
        const Value &getEntity() const;

        [[nodiscard]] std::string getString() const;
        void andPredicate();
    };
};

#endif //PROJECT_ENTITY_H
