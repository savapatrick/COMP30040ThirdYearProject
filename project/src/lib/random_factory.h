//
// Created by Patrick Sava on 12/2/2020.
//

#ifndef PROJECT_RANDOM_FACTORY_H
#define PROJECT_RANDOM_FACTORY_H

#include <string>
#include <unordered_set>
namespace utils {
class RandomFactory {
    private:
    static constexpr int RANGEMAX() {
        return 1000000000;
    }

    public:
    static std::string getRandomTermOrFunctionName(std::unordered_set<std::string>& reservedTermNames, const std::string& pref = "");
    static std::string getRandomConstantName(std::unordered_set<std::string>& reservedTermNames);
    static std::string getRandomVariableName(std::unordered_set<std::string>& reservedTermNames);
    static std::string getRandomFunctionName(std::unordered_set<std::string>& reservedTermNames);
    static std::string getRandomPredicateName(std::unordered_set<std::string>& reservedPredicateNames);
};
} // namespace utils


#endif // PROJECT_RANDOM_FACTORY_H
