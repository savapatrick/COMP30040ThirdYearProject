//
// Created by Patrick Sava on 12/2/2020.
//

#include "random_factory.h"
#include <random>

using namespace std;

namespace utils {
std::string RandomFactory::getRandomTermOrFunctionName(std::unordered_set<std::string>& reservedTermNames, const std::string& pref) {
    static std::string alphabet = "abcdefghijklmnopqrstuvwxyz";
    const int sizeOfAlphabet    = 26;
    static std::random_device RandomDevice;
    static std::mt19937 generator(RandomDevice());
    std::uniform_int_distribution<int> distribution(0, RANGEMAX());
    string result;
    do {
        result     = pref;
        int length = distribution(generator) % 15 + 1; /// 26^15 is huge
        for(int ind = 1; ind <= length; ++ind) { result += alphabet[distribution(generator) % sizeOfAlphabet]; }
    } while(reservedTermNames.find(result) != reservedTermNames.end());
    reservedTermNames.insert(result);
    return result;
}

std::string RandomFactory::getRandomPredicateName(std::unordered_set<std::string>& reservedPredicateNames) {
    static std::string startingLetterAlphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    static std::string alphabet               = "abcdefghijklmnopqrstuvwxyz";
    const int sizeOfAlphabet                  = 26;
    static std::random_device RandomDevice;
    static std::mt19937 generator(RandomDevice());
    std::uniform_int_distribution<int> distribution(0, RANGEMAX());
    string result;
    do {
        result.clear();
        int length = distribution(generator) % 15 + 1; /// 26^15 is huge
        result += startingLetterAlphabet[distribution(generator) % sizeOfAlphabet];
        for(int ind = 2; ind <= length; ++ind) { result += alphabet[distribution(generator) % sizeOfAlphabet]; }
    } while(reservedPredicateNames.find(result) != reservedPredicateNames.end());
    reservedPredicateNames.insert(result);
    return result;
}
std::string RandomFactory::getRandomConstantName(unordered_set<std::string>& reservedTermNames) {
    return getRandomTermOrFunctionName(reservedTermNames, "_c_");
}
std::string RandomFactory::getRandomVariableName(unordered_set<std::string>& reservedTermNames) {
    return getRandomTermOrFunctionName(reservedTermNames, "_v_");
}
std::string RandomFactory::getRandomFunctionName(unordered_set<std::string>& reservedTermNames) {
    return getRandomTermOrFunctionName(reservedTermNames, "_f_");
}

} // namespace utils