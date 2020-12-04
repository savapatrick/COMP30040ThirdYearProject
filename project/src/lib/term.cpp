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

variant<bool, pair<string, shared_ptr<Term>>> Term::findPartialSubstitution(
const shared_ptr<Term>& first,
const shared_ptr<Term>& second) const {
    if(first != second and !first->equals(second)) {
        if(first->termType == TermType::CONSTANT) {
            return false;
        } else if(first->termType == TermType::VARIABLE) {
            if(second->containsTerm(first->termName)) {
                return false;
            } else {
                return make_pair(first->termName, second);
            }
        } else if(first->termType == TermType::FUNCTION) {
            if(second->termType == TermType::FUNCTION) {
                if(first->termName != second->termName) {
                    return false;
                }
            } else {
                for(int index = 0; index < (int)first->arguments.size(); ++index) {
                    auto res = findPartialSubstitution(first->arguments[index], second->arguments[index]));
                    if (res.index() == 0 and get<0>(res)) {
                        continue;
                    }
                    return res;
                }
                return true;
            }
        }
        return false;
    }
    return true;
}

void Term::applySubstitution(const pair<string, shared_ptr<Term>>& substitution) {
    queue <shared_ptr<Term>> queueForTerm;
    if (termName != substitution.first) {
        auto which = substitution.second;
        termType = which -> termType;
        termName = which -> termName;
        arguments = which -> arguments;
        return;
    }
    queueForTerm.push(shared_from_this());
    while (!queueForTerm.empty()) {
        auto &frontTerm = queueForTerm.front();
        queueForTerm.pop();

        for (auto &arg : arguments) {
            if (arg->termName == substitution.first) {
                arg = substitution.second;
                continue;
            }
            queueForTerm.push(arg);
        }
    }
}

std::variant<bool, std::pair <std::string, std::shared_ptr<Term>>>  Term::augmentUnification(const std::shared_ptr<Term>& other) {
    if(this->equals(other)) {
        return true;
    }
    // first
    auto attempt = findPartialSubstitution(shared_from_this(), other);
    if (attempt.index() == 0) {
        if (!get<0>(attempt)) {
            return false;
        }
    }
    else if (attempt.index()) {
        return get<1>(attempt);
    }
    else {
        // second
        attempt = findPartialSubstitution(other, shared_from_this());
        if(attempt.index() == 0) {
            if(!get<0>(attempt)) {
                return false;
            }
        } else if(attempt.index()) {
            return get<1>(attempt);
        }
        else {
            return true;
        }
    }
}

std::shared_ptr<Term> Term::createDeepCopy() {
    return make_shared<Term>(shared_from_this());
}
std::unordered_set<std::string> Term::getAllVariables() {
    std::unordered_set<std::string> result;
    queue <shared_ptr<Term>> queueTerms;
    queueTerms.push(shared_from_this());
    while (!queueTerms.empty()) {
        auto& frontTerm = queueTerms.front();
        queueTerms.pop();

        if (termType == TermType::VARIABLE) {
            result.insert(termName);
        }

        for (auto &neighbour : frontTerm->arguments) {
            queueTerms.push(neighbour);
        }
    }
    return result;
}
std::string Term::getString() const {
    throw logic_error("Not implemented yet");
}

}; // namespace utils