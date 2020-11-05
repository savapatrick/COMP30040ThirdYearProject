//
// Created by Patrick Sava on 11/5/2020.
//

#include "entity.h"
#include <stdexcept>

using namespace std;

namespace utils {

    EntityType Entity::getType() const {
        return type;
    }

    template<typename Value>
    const Value &Entity::getEntity() const {
        throw logic_error("Not existing getter");
    }

    template<> const string &Entity::getEntity() const {
        return get<0>(entity);
    }

    // throws
    template<> const Entity::PredicateStorage &Entity::getEntity() const {
        return get<1>(entity);
    }

    // throws
    template<> const Entity::NormalFormStorage &Entity::getEntity() const {
        return get<2>(entity);
    }
};
