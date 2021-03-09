//
// Created by Patrick Sava on 12/2/2020.
//

#include "literal.h"
#include "ad_hoc_templated.h"
#include <stdexcept>

using namespace std;

namespace utils {

bool Literal::equalsWithoutSign(const std::shared_ptr<Literal>& other) {
    if(predicateName != other->predicateName) {
        return false;
    }
    if(terms.size() != other->terms.size()) {
        throw logic_error("Predicate " + predicateName + " occurs with different arities!");
    }
    for(int index = 0; index < (int)terms.size(); ++index) {
        if(!terms[index]->equals(other->terms[index])) {
            return false;
        }
    }
    return true;
}

std::shared_ptr<Literal> Literal::createDeepCopy() {
    return std::make_shared<Literal>(shared_from_this());
}

std::variant<bool, std::pair<std::string, std::shared_ptr<Term>>> Literal::augmentUnification(const std::shared_ptr<Literal>& other) {
    if(this->equalsWithoutSign(other)) {
        return true;
    }
    for(int index = 0; index < (int)other->terms.size(); ++index) {
        auto attempt = terms[index]->augmentUnification(other->terms[index]);
        if(attempt.index() == 0) {
            if(!get<0>(attempt)) {
                return false;
            }
            continue;
        } else {
            return get<1>(attempt);
        }
    }
    return true;
}

bool Literal::hasNestedFunctions() const {
    for(auto& term : terms) {
        if(term->hasNestedFunctions()) {
            return true;
        }
    }
    return false;
}

std::unordered_set<std::string> Literal::getAllVariables() const {
    unordered_set<string> result;
    for(auto& term : terms) {
        AdHocTemplated<string>::unionIterablesUnorderedSetInPlace(term->getAllVariables(), result, result);
    }
    return result;
}

void Literal::applySubstitution(const std::pair<std::string, std::shared_ptr<Term>>& mapping) {
    for(auto& term : terms) { term->applySubstitution({ mapping.first, mapping.second }); }
}

std::pair<std::string, bool> Literal::getLiteral() {
    return { predicateName, isNegated };
}

std::string Literal::getPredicateName() const {
    return predicateName;
}

std::string Literal::getTerms() const {
    string params;
    for(auto& term : terms) { params += term->getString() + ","; }
    params.pop_back();
    return params;
}

std::string Literal::getString() const {
    string sign;
    if(isNegated) {
        sign = "~";
    }
    return sign + predicateName + "(" + getTerms() + ")";
}

std::string Literal::getStringWithoutVariableNames() const {
    string sign;
    if(isNegated) {
        sign = "~";
    }
    string params;
    for(auto& term : terms) { params += term->getStringWithoutVariableNames() + ","; }
    params.pop_back();
    return sign + predicateName + "(" + params + ")";
}

void Literal::applySubstitution(const pair<std::string, std::string>& mapping) {
    for(auto& term : terms) { term->applySubstitution({ mapping.first, mapping.second }); }
}

void Literal::renameFunction(const pair<std::string, std::string>& mapping) {
    for(auto& term : terms) { term->renameFunction({ mapping.first, mapping.second }); }
}

std::pair<int, std::unordered_map<std::string, int>> Literal::getDepths() {
    std::unordered_map<string, int> result;
    int maxDepth = 0;
    for(auto& term : terms) {
        auto termDepths = term->getDepths();
        maxDepth        = max(maxDepth, termDepths.first);
        for(auto& keyValue : termDepths.second) {
            result[keyValue.first] = max(result[keyValue.first], keyValue.second);
        }
    }
    return { maxDepth, result };
}

std::string Literal::getHash(const unordered_map<std::string, std::string>& substitution) const {
    string sign;
    if(isNegated) {
        sign = "~";
    }
    string params;
    for(auto& term : terms) { params += term->getHash(substitution) + ","; }
    params.pop_back();
    return sign + predicateName + "(" + params + ")";
}

std::vector<std::string> Literal::getAllVariablesInOrder() const {
    vector<string> variablesInOrder;
    for(auto& term : terms) {
        auto currentVariablesInOrder = term->getAllVariablesInOrder();
        variablesInOrder.insert(variablesInOrder.end(), currentVariablesInOrder.begin(), currentVariablesInOrder.end());
    }
    return variablesInOrder;
}

int Literal::getArityExcludingConstants() const {
    return getAllVariablesInOrder().size();
}

bool Literal::getIsEquality() const {
    return isEquality;
}

bool Literal::getIsNegated() const {
    return isNegated;
}

void Literal::negate() {
    isNegated ^= true;
}

}; // namespace utils
