//
// Created by Patrick Sava on 12/2/2020.
//

#include "random_factory.h"

using namespace std;

namespace utils {
std::string RandomFactory::getRandomTermName(std::unordered_set<std::string>& reservedTermNames) {
    static std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    const int sizeOfAlphabet    = 26;
    // TODO: consider whether we want the C++11 random generator
    std::srand(13);
    string result;
    do {
        result.clear();
        int length = rand() % 15 + 1; /// 26^15 is huge
        for(int ind = 1; ind <= length; ++ind) { result += alphabet[rand() % sizeOfAlphabet]; }
    } while(reservedTermNames.find(result) != reservedTermNames.end());
    reservedTermNames.insert(result);
    return result;
}

std::string RandomFactory::getRandomPredicateName(std::unordered_set<std::string>& reservedPredicateNames) {
    static std::string startingLetterAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::string alphabet               = "abcdefghijklmnopqrstuvwxyz";
    const int sizeOfAlphabet                  = 26;
    // TODO: consider whether we want the C++11 random generator
    std::srand(14);
    string result;
    do {
        result.clear();
        int length = rand() % 15 + 1; /// 26^15 is huge
        result += startingLetterAlphabet[rand() % sizeOfAlphabet];
        for(int ind = 2; ind <= length; ++ind) { result += alphabet[rand() % sizeOfAlphabet]; }
    } while(reservedPredicateNames.find(result) != reservedPredicateNames.end());
    reservedPredicateNames.insert(result);
    return result;
}
}