//
// Created by Patrick Sava on 12/2/2020.
//

#include "term.h"
#include <queue>

using namespace std;

namespace utils {
bool Term::equals(const std::shared_ptr<Term>& other) const {
    if(termName != other->termName) {
        return false;
    }
    if(termType != other->termType) {
        return false;
    }
    if(arguments.size() != other->arguments.size()) {
        return false;
    }
    for(int index = 0; index < (int)arguments.size(); ++index) {
        auto& argument = arguments[index];
        if (!argument->equals(other->arguments[index])) {
            return false;
        }
    }
    return true;
}

bool Term::containsTerm(const string& name) {
    queue<shared_ptr<Term>> queueTerms;
    queueTerms.push(shared_from_this());

    while(!queueTerms.empty()) {
        auto elem = queueTerms.front();
        queueTerms.pop();

        if(elem->termName == name) {
            return true;
        }
        for(auto& neighbour : elem->arguments) { queueTerms.push(neighbour); }
    }
    return false;
}

pair<std::shared_ptr<Term>, std::shared_ptr<Term>> Term::applySubstitution(
const unordered_map<std::string, std::shared_ptr<Term>>& result,
const shared_ptr<Term>& first,
const shared_ptr<Term>& second) {
    pair<std::shared_ptr<Term>, std::shared_ptr<Term>> answer = { first, second };
    for(auto& keyValue : result) {
        auto& key   = keyValue.first;
        auto& value = keyValue.second;
        if(first->termName == key) {
            answer.first = value;
        }
        if(second->termName == key) {
            answer.second = value;
        }
    }
    return answer;
}

bool Term::findSubstitution(unordered_map<std::string, std::shared_ptr<Term>>& result,
const shared_ptr<Term>& first,
const shared_ptr<Term>& second) const {
    auto substituted = applySubstitution(result, first, second);
    if(substituted.first != substituted.second and !substituted.first->equals(substituted.second)) {
        if(result.find(first->termName) != result.end()) {
            return false;
        } else {
            if(first->termType == TermType::CONSTANT) {
                return false;
            } else if(first->termType == TermType::VARIABLE) {
                if(substituted.second->containsTerm(first->termName)) {
                    return false;
                } else {
                    result[first->termName] = substituted.second;
                    return true;
                }
            } else if(first->termType == TermType::FUNCTION) {
                if(substituted.second->termType == TermType::FUNCTION) {
                    if(first->termName != substituted.second->termName) {
                        return false;
                    }
                } else {
                    unordered_map<string, std::shared_ptr<Term>> currentSubstitution = result;
                    for(int index = 0; index < (int)first->arguments.size(); ++index) {
                        if(!findSubstitution(currentSubstitution, first->arguments[index], substituted.second->arguments[index])) {
                            return false;
                        }
                    }
                    result.swap(currentSubstitution);
                    return true;
                }
            }
            return false;
        }
    }
    return true;
}

bool Term::attemptToUnify(std::unordered_map<std::string, std::shared_ptr<Term>>& result, const std::shared_ptr<Term>& other) {
    if(this->equals(other)) {
        return true;
    }
    queue<shared_ptr<Term>> thisQueue;
    queue<shared_ptr<Term>> otherQueue;

    thisQueue.push(shared_from_this());
    otherQueue.push(other);

    while(!thisQueue.empty() and !otherQueue.empty()) {
        auto frontThis = thisQueue.front();
        thisQueue.pop();
        auto frontOther = otherQueue.front();
        otherQueue.pop();

        if(!findSubstitution(result, frontThis, frontOther)) {
            if(!findSubstitution(result, frontOther, frontThis)) {
                return false;
            }
        }

        for(auto& argument : frontThis->arguments) { thisQueue.push(argument); }

        for(auto& argument : frontOther->arguments) { otherQueue.push(argument); }
    }
    return true;
}

std::shared_ptr<Term> Term::createDeepCopy() {
    return make_shared<Term>(shared_from_this());
}
void Term::applySubstitution(unordered_map<std::string, std::shared_ptr<Term>>& substitution) {
    queue <shared_ptr<Term>> queueForTerm;
    if (substitution.find(termName) != substitution.end()) {
        auto which = substitution[termName];
        termType = which -> termType;
        termName = which -> termName;
        arguments = which -> arguments;
        return;
    }
    queueForTerm.push(shared_from_this());
    while (!queueForTerm.empty()) {
        auto &frontTerm = queueForTerm.front();
        queueForTerm.pop();

        if (substitution.find(frontTerm->termName) != substitution.end()) {
            frontTerm = substitution[frontTerm->termName];
        }

        for (auto &arg : arguments) {
            queueForTerm.push(arg);
        }
    }
}

}; // namespace utils