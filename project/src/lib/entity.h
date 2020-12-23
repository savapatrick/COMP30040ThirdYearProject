//
// Created by Patrick Sava on 11/5/2020.
//

#ifndef PROJECT_ENTITY_H
#define PROJECT_ENTITY_H

#include "simplified_clause_form.h"
#include "simplified_literal.h"
#include <memory>
#include <stdexcept>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace utils {
enum EntityType {
    BOUNDVariable = 0,  /*e.g @x, ?y, etc*/
    SIMPLIFIEDLiteral,  /*e.g ~P(a), ~P(f(x)), ~P(a, x, f(y))*/
    SIMPLIFIEDOperator, /*AND, OR, NOT, IMPLY, DOUBLEImply*/
    NORMALForms,        /*e.g CNF or DNF, so either ~P(a) ^ B(d), or ~P(d) | P(f) could be good examples*/
};

class Entity {
    public:
    typedef std::variant<std::string, /*type 0 or type 2*/
    std::shared_ptr<SimplifiedLiteral> /*type 1*/,
    std::shared_ptr<SimplifiedClauseForm> /*type 3*/>
    EntityStorage;

    private:
    EntityType type;
    EntityStorage entity;

    public:
    Entity() = default;

    Entity(const std::shared_ptr<Entity>& other) : type(other->type) {
        if (type == 0 or type == 2) {
            entity = std::get<0>(other->entity);
        }
        else if (type == 1) {
            entity = std::make_shared<SimplifiedLiteral>(std::get<1>(other->entity));
        }
        else {
            throw std::logic_error("Not implemented because simplified clause form should never be duplicated"
                                   "as part of entity");
        }
    }

    explicit Entity(const EntityType& _type, const std::string& _entity) : type(_type), entity(_entity) {
        if(type and type != 2) {
            throw std::invalid_argument("Type for Entity has to be 0 or 2 but given " + std::to_string(type));
        }
    };

    explicit Entity(const EntityType& _type, const std::shared_ptr<SimplifiedClauseForm>& _entity)
    : type(_type), entity(_entity) {
        if(type != 3) {
            throw std::invalid_argument("Type for Entity has to be 3 but given " + std::to_string(type));
        }
    };

    explicit Entity(const EntityType& _type, const std::shared_ptr<SimplifiedLiteral>& _entity)
    : type(_type), entity(_entity) {
        if(type != 1) {
            throw std::invalid_argument("Type for Entity has to be 1 but given " + std::to_string(type));
        }
    };

    [[nodiscard]] EntityType getType() const;

    template <typename Value> Value getEntity() const;

    [[nodiscard]] std::string getString() const;

    void applySubstitution(std::unordered_map<std::string, std::string>& substitution);
};
}; // namespace utils

#endif // PROJECT_ENTITY_H
