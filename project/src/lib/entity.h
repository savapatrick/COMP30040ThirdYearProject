//
// Created by Patrick Sava on 11/5/2020.
//

#ifndef PROJECT_ENTITY_H
#define PROJECT_ENTITY_H

#include <string>
#include <utility>
#include <vector>
#include <variant>

namespace utils {
    enum EntityType {
        BOUNDVariable = 0, /*e.g @x, ?y, etc*/
        PREDICATE, /*e.g ~P(a), ~P(f(x)), ~P(a, x, f(y))*/
        SIMPLIFIEDOperator, /*AND, OR, NOT, IMPLY*/
        NORMALForms, /*e.g CNF or DNF, so either ~P(a) ^ B(d), or ~P(d) | P(f) could be good examples*/
    };
    class Entity{
    public:
        typedef std::pair<bool, std::pair<std::string, std::vector<std::string>>> PredicateStorage;
        typedef std::pair<bool, std::vector<PredicateStorage>> NormalFormStorage;
        typedef std::variant<std::string, /*type 0*/
                PredicateStorage , /*type 1*/
                std::string, /*type 2*/
                NormalFormStorage /*type 3*/> EntityStorage;
    private:
        EntityType type;
        EntityStorage entity;
    public:
        Entity() = default;
        explicit Entity(const EntityType &_type, EntityStorage _entity) : type(_type), entity(std::move(_entity)){};
        [[nodiscard]] EntityType getType() const;

        template<typename Value>
        const Value &getEntity() const;
    };
};

#endif //PROJECT_ENTITY_H
