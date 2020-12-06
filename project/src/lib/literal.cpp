//
// Created by Patrick Sava on 12/2/2020.
//

#include "literal.h"
#include "ad_hoc_templated.h"
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

std::shared_ptr<Literal> Literal::createDeepCopy() {
    return std::shared_ptr<Literal>(shared_from_this());
}

std::variant<bool, std::pair <std::string, std::shared_ptr<Term>>> Literal::augmentUnification(const std::shared_ptr<Literal>& other) {
    if (this->equalsWithoutSign(other)) {
        return true;
    }
    for (int index = 0; index < (int)other->terms.size(); ++ index) {
        auto attempt = terms[index]->augmentUnification(other->terms[index]);
        if (attempt.index() == 0) {
            if (!get<0>(attempt)) {
                return false;
            }
            continue;
        }
        else {
            return get<1>(attempt);
        }
    }
    return true;
}
std::unordered_set<std::string> Literal::getAllVariables() {
    unordered_set<string> result;
    for (auto &term : terms) {
        AdHocTemplated<string>::unionIterablesUnorderedSetInPlace(term->getAllVariables(), result, result);
    }
    return result;
}

void Literal::applySubstitution(const std::pair <std::string, std::shared_ptr<Term>> &mapping) {
    for (auto &term : terms) {
        term->applySubstitution({mapping.first, mapping.second});
    }
}
std::pair<std::string, bool> Literal::getLiteral() {
    return {predicateName, isNegated};
}
std::string Literal::getString() const {
    string sign;
    if (isNegated) {
        sign = "~";
    }
    string params;
    for (auto &term : terms) {
        params += term->getString() + ",";
    }
    params.pop_back();
    return sign + predicateName + "(" + params + ")";
}

};
