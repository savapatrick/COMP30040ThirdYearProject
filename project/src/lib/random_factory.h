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
    RandomFactory(){};
    RandomFactory(RandomFactory const&);
    void operator=(RandomFactory const&);

    public:
    static RandomFactory& getInstance() {
        static RandomFactory instance;
        return instance;
    }

    static std::string getRandomTermName(std::unordered_set<std::string>& reservedTermNames);
    static std::string getRandomPredicateName(std::unordered_set<std::string>& reservedPredicateNames);
};
}


#endif // PROJECT_RANDOM_FACTORY_H
