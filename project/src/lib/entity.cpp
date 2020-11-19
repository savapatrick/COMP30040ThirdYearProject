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
template <> shared_ptr<Literal> Entity::getEntity() const {
    return get<1>(entity);
}

// throws
template <> shared_ptr<ClauseForm> Entity::getEntity() const {
    return get<2>(entity);
}

std::string Entity::getString() const {
    Operators& operators = Operators::getInstance();
    if(type == EntityType::BOUNDVariable or type == EntityType::SIMPLIFIEDOperator) {
        return get<0>(entity);
    } else if(type == EntityType::LITERAL) {
        return get<1>(entity)->getString();
    } else {
        return get<2>(entity)->getString();
    }
}
}; // namespace utils
