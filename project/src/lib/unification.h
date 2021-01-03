//
// Created by Patrick Sava on 12/29/2020.
//

#ifndef PROJECT_UNIFICATION_H
#define PROJECT_UNIFICATION_H

#include "clause.h"
#include "clause_form.h"

#include <fstream>
#include <utility>

namespace utils {
class Unification {
    private:
    std::ostream& outputStream;

    public:
    Unification(std::ostream& stream) : outputStream(stream){};
    std::variant<bool, std::shared_ptr<Clause>> tryToUnifyTwoLiterals(std::shared_ptr<Clause>& clause); // and commit if possible
    template <typename LiteralPredicate, typename ResolventPredicate>
    std::pair<bool, std::shared_ptr<Clause>> attemptToUnify(std::shared_ptr<Clause>& first,
    std::shared_ptr<Clause>& second,
    LiteralPredicate literalPredicate,
    ResolventPredicate resolventPredicate);
};
template <typename LiteralPredicate, typename ResolventPredicate>
std::pair<bool, std::shared_ptr<Clause>> Unification::attemptToUnify(std::shared_ptr<Clause>& first,
std::shared_ptr<Clause>& second,
LiteralPredicate literalPredicate,
ResolventPredicate resolventPredicate) {
    auto literalsFirst  = first->getAllLiterals();
    auto literalsSecond = second->getAllLiterals();
    bool ok             = false;
    for(auto& keyValue : literalsFirst) {
        if(literalsSecond.find({ keyValue.first.first, 1 - keyValue.first.second }) != literalsSecond.end()) {
            ok = true;
        }
    }
    if(!ok) {
        return { false, std::shared_ptr<Clause>(nullptr) };
    }
    std::map<std::pair<int, int>, bool> blackListed;
    bool found;
    do {
        found = false;
        std::pair<int, int> indexes;
        for(int index = 0; index < (int)first->clause.size() and !found; ++index) {
            for(int index2 = 0; index2 < (int)second->clause.size() and !found; ++index2) {
                if(literalPredicate(first->clause[index], second->clause[index2])) {
                    indexes = { index, index2 };
                    if(blackListed.find(indexes) != blackListed.end()) {
                        continue;
                    }
                    found                = true;
                    blackListed[indexes] = true;
                }
            }
        }
        if(found) {
            bool unified;
            auto firstDeepCopy  = first->createDeepCopy();
            auto secondDeepCopy = second->createDeepCopy();
            do {
                auto result = firstDeepCopy->clause[indexes.first]->augmentUnification(secondDeepCopy->clause[indexes.second]);
                if(result.index() == 0) {
                    if(std::get<0>(result)) {
                        unified = true;
                        break;
                    } else {
                        unified = false;
                        break;
                    }
                } else {
                    firstDeepCopy->applySubstitution(std::get<1>(result));
                    secondDeepCopy->applySubstitution(std::get<1>(result));
                }
            } while(true);
            if(unified) {
                outputStream << "clauses " + firstDeepCopy->getString() + " and " + secondDeepCopy->getString()
                             << " get resolution rule applied on " + firstDeepCopy->clause[indexes.first]->getString() +
                " and on " + secondDeepCopy->clause[indexes.second]->getString() + "\n[ADD] the resulting clause ";
                auto resolvedLiteral = firstDeepCopy->clause[indexes.first];
                firstDeepCopy->clause.erase(firstDeepCopy->clause.begin() + indexes.first);
                secondDeepCopy->clause.erase(secondDeepCopy->clause.begin() + indexes.second);
                for(auto& literal : secondDeepCopy->clause) { firstDeepCopy->clause.push_back(literal); }
                outputStream << firstDeepCopy->getString() << " is added to the set of clauses\n";
                if(resolventPredicate(resolvedLiteral, firstDeepCopy->clause)) {
                    return { true, firstDeepCopy };
                }
            }
        }
    } while(found);
    return { false, std::shared_ptr<Clause>(nullptr) };
}
}; // namespace utils

#endif // PROJECT_UNIFICATION_H
