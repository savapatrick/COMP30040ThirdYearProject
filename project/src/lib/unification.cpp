//
// Created by Patrick Sava on 12/29/2020.
//

#include "unification.h"

using namespace std;

namespace utils {

std::vector<std::shared_ptr<Clause>> Unification::tryToUnifyTwoLiterals(std::shared_ptr<Clause>& initialClause) {
    shared_ptr<Clause> clause = initialClause->createDeepCopy();
    auto clauseLiterals       = clause->getLiteralsAndCount();
    bool ok                   = false;
    for(auto& keyValue : clauseLiterals) {
        if(keyValue.second > 1) {
            ok = true;
            break;
        }
    }
    if(!ok) {
        return std::vector<std::shared_ptr<Clause>>({});
        ;
    }
    pair<int, int> indexes;
    std::vector<std::shared_ptr<Clause>> clauses;
    for(int index = 0; index < (int)clause->clause.size(); ++index) {
        for(int index2 = index + 1; index2 < (int)clause->clause.size(); ++index2) {
            if(clause->clause[index]->predicateName == clause->clause[index2]->predicateName and
            clause->clause[index]->isNegated == clause->clause[index2]->isNegated) {
                indexes = { index, index2 };
                bool unified;
                auto clauseDeepCopy = clause->createDeepCopy();
                do {
                    auto result =
                    clauseDeepCopy->clause[indexes.first]->augmentUnification(clauseDeepCopy->clause[indexes.second]);
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
                                 << " because literals " + clauseDeepCopy->clause[indexes.first]->getString() +
                    " and " + clauseDeepCopy->clause[indexes.second]->getString() + " we're succesfully unified\n";
                    clauses.push_back(clauseDeepCopy);
                }
            }
        }
    }
    return clauses;
}

}; // namespace utils