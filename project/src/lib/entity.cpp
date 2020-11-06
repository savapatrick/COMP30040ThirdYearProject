//
// Created by Patrick Sava on 11/5/2020.
//

#include "entity.h"
#include <stdexcept>
#include "operators.h"

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
    template<> const Literal &Entity::getEntity() const {
        return get<1>(entity);
    }

    // throws
    template<> const Entity::NormalFormStorage &Entity::getEntity() const {
        return get<2>(entity);
    }

    std::string Entity::getString() const {
        Operators& operators = Operators::getInstance();
        if (type == EntityType::BOUNDVariable or type == EntityType::SIMPLIFIEDOperator) {
            return get<0>(entity);
        }
        else if (type == EntityType::LITERAL) {
            return get<1>(entity).getString();
        }
        else {
            auto info = get<2>(entity);
            string op = operators.OR;
            if (info.first == NormalFormType::CNF) {
                op = operators.AND;
            }
            string result;
            for (int ind = 0; ind < (int)info.second.size(); ++ ind) {
                if (ind + 1 == info.second.size()) {
                    result += info.second[ind].getString();
                }
                else {
                    result += info.second[ind].getString() + op;
                }
            }
            return result;
        }
    }

    void Entity::andPredicate() {
        if (type == 0)
    }

};
