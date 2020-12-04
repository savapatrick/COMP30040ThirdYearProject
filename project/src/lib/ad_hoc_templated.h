//
// Created by Patrick Sava on 12/3/2020.
//

#ifndef PROJECT_AD_HOC_TEMPLATED_H
#define PROJECT_AD_HOC_TEMPLATED_H

#include <string>
#include <unordered_set>
#include <vector>

namespace utils {
template <class T, class V = std::string> class AdHocTemplated {
    private:
    public:
    template <template <class, typename...> class P, template <class, typename...> class Q>
    static std::vector<T> unionIterablesVector(const P<V>& first, const Q<V>& second) {
        std::vector<T> resultVector;
        std::unordered_set<T> partialResult;
        for (auto &elem : first) {
            partialResult.insert(elem);
        }
        for (auto &elem : second) {
            partialResult.insert(elem);
        }
        for (auto &elem: partialResult) {
            resultVector.push_back(elem);
        }
        return resultVector;
    }

    template <template <class, typename...> class P, template <class, typename...> class Q>
    static std::unordered_set<T> unionIterablesUnorderedSet(const P<V>& first, const Q<V>& second) {
        std::unordered_set<T> result;
        for (auto &elem : first) {
            result.insert(elem);
        }
        for (auto &elem : second) {
            result.insert(elem);
        }
        return result;
    }

    template <template <class, typename...> class P, template <class, typename...> class Q, template <class, typename...> class S>
    static void unionIterablesUnorderedSetInPlace(const P<V>& first, const Q<V>& second, S<T>& result) {
        for (auto &elem : first) {
            result.insert(elem);
        }
        for (auto &elem : second) {
            result.insert(elem);
        }
    }
};

} // namespace utils

#endif // PROJECT_AD_HOC_TEMPLATED_H
