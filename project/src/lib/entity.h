//
// Created by Patrick Sava on 11/5/2020.
//

#ifndef PROJECT_ENTITY_H
#define PROJECT_ENTITY_H

#include "literal.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace utils {
enum EntityType {
    BOUNDVariable = 0,  /*e.g @x, ?y, etc*/
    LITERAL,            /*e.g ~P(a), ~P(f(x)), ~P(a, x, f(y))*/
    SIMPLIFIEDOperator, /*AND, OR, NOT, IMPLY, DOUBLEImply*/
    NORMALForms,        /*e.g CNF or DNF, so either ~P(a) ^ B(d), or ~P(d) | P(f) could be good examples*/
};
enum NormalFormType {
    CNF = 0,
    DNF,
};

class Entity {
    public:
    typedef std::pair<NormalFormType, std::vector<Literal>> NormalFormStorage;
    typedef std::variant<std::string, /*type 0 or type 2*/
    std::shared_ptr<Literal>,
    NormalFormStorage /*type 3*/>
    EntityStorage;

    private:
    EntityType type;
    EntityStorage entity;

    public:
    Entity () = default;

    explicit Entity (const EntityType& _type, const std::string& _entity) : type (_type), entity (_entity) {
        if (type and type != 2) {
            throw std::invalid_argument ("Type for Entity has to be 0 or 2 but given " + std::to_string (type));
        }
    };

    explicit Entity (const EntityType& _type, const NormalFormStorage& _entity) : type (_type), entity (_entity) {
        if (type != 3) {
            throw std::invalid_argument ("Type for Entity has to be 3 but given " + std::to_string (type));
        }
    };

    explicit Entity (const EntityType& _type, const std::shared_ptr<Literal>& _entity)
    : type (_type), entity (_entity) {
        if (type != 1) {
            throw std::invalid_argument ("Type for Entity has to be 1 but given " + std::to_string (type));
        }
    };

    [[nodiscard]] EntityType getType () const;

    template <typename Value> Value getEntity () const;

    [[nodiscard]] std::string getString () const;
};
}; // namespace utils

#endif // PROJECT_ENTITY_H
