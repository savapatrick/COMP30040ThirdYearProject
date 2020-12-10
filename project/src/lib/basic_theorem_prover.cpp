//
// Created by Patrick Sava on 11/25/2020.
//

#include "basic_theorem_prover.h"
#include <algorithm>
#include <random>

using namespace std;

namespace utils {

bool BasicTheoremProver::tryToUnifyTwoLiterals(std::shared_ptr<Clause>& clause) {
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
        for(int index = 0; index < (int)clause->clause.size(); ++index) {
            for(int index2 = index + 1; index2 < (int)clause->clause.size(); ++index2) {
                if(clause->clause[index]->predicateName == clause->clause[index2]->predicateName) {
                    indexes = { index, index2 };
                    if(blackListed.find(indexes) != blackListed.end()) {
                        continue;
                    }
                    found = true;
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
                outputStream.flush();
                clause             = clauseDeepCopy;
                unifiedAtLeastOnce = true;
            } else {
                blackListed[indexes] = true;
            }
        }
    } while(found);
    return unifiedAtLeastOnce;
}
bool BasicTheoremProver::isTautology(std::shared_ptr<Clause>& clause) {
    unordered_map<string, vector<shared_ptr<Literal>>> literals;
    for(auto& currentLiteral : clause->clause) {
        for(auto& literal : literals[currentLiteral->predicateName]) {
            if(currentLiteral->equalsWithoutSign(literal)) {
                if(currentLiteral->isNegated != literal->isNegated) {
                    return true;
                }
            }
        }
        literals[currentLiteral->predicateName].push_back(currentLiteral);
    }
    return false;
}
bool BasicTheoremProver::removeDuplicates(std::shared_ptr<Clause>& clause) {
    unordered_map<string, shared_ptr<Literal>> literals;
    for(auto& literal : clause->clause) { literals[literal->getString()] = literal; }
    outputStream << "removing duplicates from clause " + clause->getString() << "\n";
    outputStream.flush();
    if(literals.size() != clause->clause.size()) {
        clause->clause.clear();
        clause->clause.reserve(literals.size());
        for(auto& keyValue : literals) { clause->clause.emplace_back(keyValue.second); }
        outputStream << "it then becomes " << clause->getString() << "\n";
        outputStream.flush();
        return true;
    }
    return false;
}

bool BasicTheoremProver::factoringStep() {
    bool changed = false;
    std::vector<std::shared_ptr<Clause>> newClauseForm;
    for(auto& clause : clauseForm->clauseForm) {
        changed |= removeDuplicates(clause);
        newClauseForm.push_back(clause);
        if(tryToUnifyTwoLiterals(clause)) {
            if(isTautology(clause)) {
                outputStream << "clause " + clause->getString() + " is a tautology, so it's dropped\n";
                outputStream.flush();
                newClauseForm.pop_back();
                changed = true;
            }
        }
    }
    if(changed) {
        clauseForm->clauseForm = newClauseForm;
    }
    if(clauseForm->clauseForm.empty()) {
        return true;
    }
    return false;
}

std::pair<bool, std::shared_ptr<Clause>>
BasicTheoremProver::attemptToUnify(std::shared_ptr<Clause>& first, std::shared_ptr<Clause>& second) {
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
        for(int index = 0; index < (int)first->clause.size(); ++index) {
            for(int index2 = 0; index2 < (int)second->clause.size(); ++index2) {
                if(first->clause[index]->isNegated != second->clause[index2]->isNegated) {
                    if(first->clause[index]->predicateName == second->clause[index2]->predicateName) {
                        indexes = { index, index2 };
                        if(blackListed.find(indexes) != blackListed.end()) {
                            continue;
                        }
                        found = true;
                    }
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
                outputStream.flush();
                firstDeepCopy->clause.erase(firstDeepCopy->clause.begin() + indexes.first);
                secondDeepCopy->clause.erase(secondDeepCopy->clause.begin() + indexes.second);
                for(auto& literal : secondDeepCopy->clause) { firstDeepCopy->clause.push_back(literal); }
                outputStream << firstDeepCopy->getString() << " is added to the set of clauses\n";
                outputStream.flush();
                return { true, firstDeepCopy };
            } else {
                blackListed[indexes] = true;
            }
        }
    } while(found);
    return { false, shared_ptr<Clause>(nullptr) };
}

bool BasicTheoremProver::resolutionStep() {
    bool found;
    do {
        found = false;
        shared_ptr<Clause> newClause;
        for(int index = 0; index < (int)clauseForm->clauseForm.size() and !found; ++index) {
            for(int index2 = index + 1; index2 < (int)clauseForm->clauseForm.size() and !found; ++index2) {
                if(avoid.find({ index, index2 }) != avoid.end()) {
                    continue;
                }
                auto result = attemptToUnify(clauseForm->clauseForm[index], clauseForm->clauseForm[index2]);
                //                outputStream << "[DEBUG] " << index << " " << index2 << " were processed\n";
                //                outputStream.flush();
                avoid.insert({ index, index2 });
                if(result.first) {
                    //                    outputStream << "[DEBUG] we managed to unify " << clauseForm->clauseForm[index]->getString() <<
                    //                    " with " << clauseForm->clauseForm[index2]->getString() << "and it results a new clause " <<
                    //                    result.second->getString() << '\n';
                    outputStream.flush();
                    found     = true;
                    newClause = result.second;
                    if(newClause->clause.empty()) {
                        // we derived the empty clause
                        return true;
                    }
                }
            }
        }
        if(found) {
            clauseForm->clauseForm.push_back(newClause);
            clauseForm->makeVariableNamesUniquePerClause();
        }
    } while(found);
    return false;
}

void BasicTheoremProver::run() {
    do {
        if(factoringStep()) {
            outputStream << "derived empty clause!\n";
            outputStream.flush();
            break;
        }
        if(resolutionStep()) {
            outputStream << "derived empty clause!\n";
            outputStream.flush();
            break;
        }
    } while(true);
}
}; // namespace utils
