//
// Created by Patrick Sava on 11/25/2020.
//

#ifndef PROJECT_DUAL_HASH_ASCII_H
#define PROJECT_DUAL_HASH_ASCII_H

#include "simplified_literal.h"
#include <memory>
#include <string>
#include <utility>

namespace utils {
class DualHashASCII {
    public:
    typedef std::pair<int, int> HashType;

    private:
    DualHashASCII() {
    }
    static const int ModuloOne = 1000000007;
    static const int ModuloTwo = 1000000009;
    static const int Base      = 197;
    static bool isASCII(const char& character);
    static HashType appendCharacter(const HashType& currentHash, char newChar);
    static HashType appendString(const HashType& currentHash, const std::string& sequence);
    static HashType appendArgument(const HashType& currentHash, const SimplifiedLiteral::arg& argument);
    static HashType appendArguments(const HashType& currentHash, const std::shared_ptr<SimplifiedLiteral>& simplifiedLiteral);

    public:
    static HashType getHashPredicateName(const std::shared_ptr<SimplifiedLiteral>& simplifiedLiteral);
    static HashType getHashArguments(const std::shared_ptr<SimplifiedLiteral>& simplifiedLiteral);
    static HashType getHash(const std::shared_ptr<SimplifiedLiteral>& simplifiedLiteral);
};
} // namespace utils

#endif // PROJECT_DUAL_HASH_ASCII_H
