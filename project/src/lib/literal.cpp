//
// Created by Patrick Sava on 12/2/2020.
//

#include "literal.h"
#include <stdexcept>

using namespace std;

namespace utils {
bool Literal::equalsWithoutSign(const std::shared_ptr<Literal>& other) {
    if (predicateName != other->predicateName) {
        return false;
    }
    if (terms.size() != other->terms.size()) {
        throw logic_error("Predicate " + predicateName + " occurs with different arities!");
    }
    for (int index = 0; index <(int)terms.size(); ++ index) {
        if(!terms[index]->equals(other->terms[index])) {
            return false;
        }
    }
    return true;
}

bool Literal::unify(const std::shared_ptr<Literal>& other) {
    if (wontWork.find(other) != wontWork.end()) {
        return false;
    }
    while (wontWork.size() > CacheLIMIT()) {
        wontWork.erase(wontWork.begin());
    }
    if (equalsWithoutSign(other)) {

    }
}
};
