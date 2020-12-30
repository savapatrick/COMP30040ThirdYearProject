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
    bool tryToUnifyTwoLiterals(std::shared_ptr<Clause>& clause); // and commit if possible
    template <class LiteralPredicate, class ResolventPredicate>
    std::pair<bool, std::shared_ptr<Clause>>
    attemptToUnify(std::shared_ptr<Clause>& first, std::shared_ptr<Clause>& second, LiteralPredicate literalPredicate,
    ResolventPredicate resolventPredicate);
};
}; // namespace utils

#endif // PROJECT_UNIFICATION_H
