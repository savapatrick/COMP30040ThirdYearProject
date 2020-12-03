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
        set_union(first.begin(), first.end(), second.begin(), second.end(), back_inserter(resultVector));
        return resultVector;
    }

    template <template <class, typename...> class P, template <class, typename...> class Q>
    static std::unordered_set<T> unionIterablesUnorderedSet(const P<V>& first, const Q<V>& second) {
        std::vector<T> resultVector;
        set_union(first.begin(), first.end(), second.begin(), second.end(), back_inserter(resultVector));
        std::unordered_set<T> result(resultVector.begin(), resultVector.end());
        return result;
    }
};

} // namespace utils

#endif // PROJECT_AD_HOC_TEMPLATED_H
