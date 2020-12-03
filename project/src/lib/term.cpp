//
// Created by Patrick Sava on 12/2/2020.
//

#include "term.h"
#include <queue>

using namespace std;

namespace utils {
bool Term::equals(const std::shared_ptr<Term> &other) const {
    if (termName != other->termName) {
        return false;
    }
    if (termType != other->termType) {
        return false;
    }
    if (arguments.size() != other->arguments.size()) {
        return false;
    }
    bool ok = true;
    for (int index = 0; index < (int)arguments.size(); ++ index) {
        auto &argument = arguments[index];
        ok &= argument->equals(other->arguments[index]);
        if (!ok) {
            return false;
        }
    }
    return true;
}

bool Term::containsTerm(const string& name) {
    queue <shared_ptr<Term>> queueTerms;
    queueTerms.push(shared_from_this());

    while (!queueTerms.empty()) {
        auto elem = queueTerms.front();
        queueTerms.pop();

        if (elem->termName == name) {
            return false;
        }
        for (auto &neighbour : elem->arguments) {
            queueTerms.push(neighbour);
        }
    }
    return true;
}

bool Term::attemptToUnify(std::map<std::shared_ptr<Term>, std::shared_ptr<Term>>& result, const std::shared_ptr<Term>& other) {
    if (this->equals(other)) {
        return true;
    }
    queue<shared_ptr<Term>> thisQueue;
    queue<shared_ptr<Term>> otherQueue;

    thisQueue.push(shared_from_this());
    otherQueue.push(other);

    auto applySubstitution = [&result](const std::shared_ptr<Term>& first, const std::shared_ptr<Term>& second)
    -> pair<std::shared_ptr<Term>, std::shared_ptr<Term>> {
        pair <std::shared_ptr<Term>, std::shared_ptr<Term>> answer = {first, second};
        for (auto &keyValue : result) {
            auto& key = keyValue.first;
            auto& value = keyValue.second;
            if (first->equals(key)) {
                answer.first = value;
            }
            if (second->equals(key)) {
                answer.second = value;
            }
        }
        return answer;
    };

    while (!thisQueue.empty() and !otherQueue.empty()) {
        auto frontThis = thisQueue.front(); thisQueue.pop();
        auto frontOther = otherQueue.front(); otherQueue.pop();

        auto substituted = applySubstitution(frontThis, frontOther);
        if (frontThis->termName != frontOther->termName) {

        }
    }
}

std::shared_ptr<Term> Term::createDeepCopy() {
    return make_shared<Term>(shared_from_this());
}

}; // namespace utils