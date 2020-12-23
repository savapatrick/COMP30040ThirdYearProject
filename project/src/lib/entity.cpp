//
// Created by Patrick Sava on 11/5/2020.
//

#include "entity.h"
#include "operators.h"
#include <stdexcept>

using namespace std;

namespace utils {

EntityType Entity::getType() const {
    return type;
}

template <typename Value> Value Entity::getEntity() const {
    throw logic_error("Not existing getter");
}

template <> string Entity::getEntity() const {
    return get<0>(entity);
}

// throws
template <> shared_ptr<SimplifiedLiteral> Entity::getEntity() const {
    return get<1>(entity);
}

// throws
template <> shared_ptr<SimplifiedClauseForm> Entity::getEntity() const {
    return get<2>(entity);
}

std::string Entity::getString() const {
    Operators& operators = Operators::getInstance();
    if(type == EntityType::BOUNDVariable or type == EntityType::SIMPLIFIEDOperator) {
        return get<0>(entity);
    } else if(type == EntityType::SIMPLIFIEDLiteral) {
        return get<1>(entity)->getString();
    } else {
        return get<2>(entity)->getString();
    }
}
void Entity::applySubstitution(std::unordered_map<std::string, std::string>& substitution) {
    if (type >= 2) {
        throw std::logic_error("cannot apply substitution on operators or normal forms!");
    }
    else if (type == 0) {
        auto variable = get<0>(entity).substr(1);
        if (substitution.find(variable) != substitution.end()) {
            variable = substitution[variable];
        }
        entity = get<0>(entity)[0] + variable;
    }
    else {
        auto literal = get<1>(entity);
        literal->simpleSubstitution(substitution);
    }
}
}; // namespace utils
