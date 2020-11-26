//
// Created by Patrick Sava on 11/25/2020.
//

#include "dual_hash_ascii.h"

using namespace std;

namespace utils {

bool DualHashASCII::isASCII(const char& character) {
    return 0 <= static_cast<int>(character) and static_cast<int>(character) <= 127;
}

DualHashASCII::HashType DualHashASCII::appendCharacter(const DualHashASCII::HashType& currentHash, const char newChar) {
    if(!isASCII(newChar)) {
        throw invalid_argument("the characters should be part of the standard ASCII");
    }
    return { (1LL * currentHash.first * Base + newChar) % ModuloOne, (1LL * currentHash.second * Base + newChar) % ModuloTwo };
}
DualHashASCII::HashType DualHashASCII::appendString(const DualHashASCII::HashType& currentHash, const std::string& sequence) {
    HashType newHash(currentHash);
    for(auto& currentCharacter : sequence) { newHash = appendCharacter(newHash, currentCharacter); }
    return newHash;
}
DualHashASCII::HashType DualHashASCII::appendArgument(const DualHashASCII::HashType& currentHash, const Literal::arg& argument) {
    if(argument.index() == 0) {
        return appendString(currentHash, get<0>(argument));
    }
    HashType newHash(currentHash);
    newHash    = appendString(newHash, get<1>(argument).first);
    newHash    = appendString(newHash, "(");
    auto& args = get<1>(argument).second;
    for(int index = 0; index < (int)args.size(); ++index) {
        auto variable = args[index];
        newHash       = appendString(newHash, variable);
        if(index + 1 < (int)args.size()) {
            newHash = appendString(newHash, ",");
        } else {
            newHash = appendString(newHash, ")");
        }
    }
    return newHash;
}

DualHashASCII::HashType
DualHashASCII::appendArguments(const DualHashASCII::HashType& currentHash, const std::shared_ptr<Literal>& literal) {
    auto& arguments = literal->arguments;
    HashType newHash(currentHash);
    newHash = appendString(newHash, "[");
    for(int index = 0; index < (int)arguments.size(); ++index) {
        newHash = appendArgument(newHash, arguments[index]);
        if(index + 1 < (int)arguments.size()) {
            newHash = appendString(newHash, ",");
        } else {
            newHash = appendString(newHash, "]");
        }
    }
    return newHash;
}

DualHashASCII::HashType DualHashASCII::getHashPredicateName(const std::shared_ptr<Literal>& literal) {
    return appendString({ 0, 0 }, literal->predicateName);
}
DualHashASCII::HashType DualHashASCII::getHashArguments(const std::shared_ptr<Literal>& literal) {
    return appendArguments({ 0, 0 }, literal);
}
DualHashASCII::HashType DualHashASCII::getHash(const std::shared_ptr<Literal>& literal) {
    HashType newHash(0, 0);
    string prefix;
    if(literal->isNegated) {
        prefix = "~";
    }
    newHash = appendString(newHash, "{" + prefix + literal->getPredicateName());
    newHash = appendArguments(newHash, literal);
    newHash = appendString(newHash, "}");
    return newHash;
}

}; // namespace utils