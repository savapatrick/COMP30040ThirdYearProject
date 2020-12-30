//
// Created by Patrick Sava on 12/29/2020.
//

#include "unification.h"

using namespace std;

namespace utils {

bool Unification::tryToUnifyTwoLiterals(std::shared_ptr<Clause>& clause) {
    auto clauseLiterals = clause->getAllLiterals();
    bool ok             = false;
    for(auto& keyValue : clauseLiterals) {
        if(keyValue.second > 1 or
        clauseLiterals.find({ keyValue.first.first, 1 - keyValue.first.second }) != clauseLiterals.end()) {
            ok = true;
            break;
        }
    }
    if(!ok) {
        return false;
    }
    map<pair<int, int>, bool> blackListed;
    bool unifiedAtLeastOnce = false;
    bool found;
    do {
        pair<int, int> indexes;
        found = false;
        for(int index = 0; index < (int)clause->clause.size() and !found; ++index) {
            for(int index2 = index + 1; index2 < (int)clause->clause.size() and !found; ++index2) {
                if(clause->clause[index]->predicateName == clause->clause[index2]->predicateName) {
                    indexes = { index, index2 };
                    if(blackListed.find(indexes) != blackListed.end()) {
                        continue;
                    }
                    blackListed[indexes] = true;
                    found                = true;
                }
            }
        }
        if(found) {
            bool unified;
            auto clauseDeepCopy = clause->createDeepCopy();
            do {
                auto result = clauseDeepCopy->clause[indexes.first]->augmentUnification(clauseDeepCopy->clause[indexes.second]);
                if(result.index() == 0) {
                    if(get<0>(result)) {
                        unified = true;
                        break;
                    } else {
                        unified = false;
                        break;
                    }
                } else {
                    clauseDeepCopy->applySubstitution(get<1>(result));
                }
            } while(true);
            if(unified) {
                outputStream << "clause " << clause->getString() << " is transformed into " + clauseDeepCopy->getString()
                             << " because literals " + clauseDeepCopy->clause[indexes.first]->getString() + " and " +
                clauseDeepCopy->clause[indexes.second]->getString() + " we're succesfully unified\n";
                clause             = clauseDeepCopy;
                unifiedAtLeastOnce = true;
            }
        }
    } while(found);
    return unifiedAtLeastOnce;
}

template <class LiteralPredicate, class ResolventPredicate>
std::pair<bool, std::shared_ptr<Clause>> Unification::attemptToUnify(shared_ptr<Clause>& first,
shared_ptr<Clause>& second,
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
        return { false, shared_ptr<Clause>(nullptr) };
    }
    map<pair<int, int>, bool> blackListed;
    bool found;
    do {
        found = false;
        pair<int, int> indexes;
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
                    if(get<0>(result)) {
                        unified = true;
                        break;
                    } else {
                        unified = false;
                        break;
                    }
                } else {
                    firstDeepCopy->applySubstitution(get<1>(result));
                    secondDeepCopy->applySubstitution(get<1>(result));
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
    return { false, shared_ptr<Clause>(nullptr) };
}

}; // namespace utils