//
// Created by Patrick Sava on 12/29/2020.
//

#include "unification.h"

using namespace std;

namespace utils {

variant<bool, shared_ptr<Clause>> Unification::tryToUnifyTwoLiterals(std::shared_ptr<Clause>& initialClause) {
    shared_ptr<Clause> clause = initialClause->createDeepCopy();
    auto clauseLiterals       = clause->getAllLiterals();
    bool ok                   = false;
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
    if(unifiedAtLeastOnce) {
        return clause;
    }
    return false;
}

}; // namespace utils