//
// Created by Patrick Sava on 11/4/2020.
//

#ifndef PROJECT_OPERATORS_H
#define PROJECT_OPERATORS_H

#include <set>
#include <string>
#include <unordered_map>

namespace utils {
// singleton class for tokens
class Operators {
    public:
    const std::string AND;
    const std::string OR;
    const std::string NOT;
    const std::string OPENEDBracket;
    const std::string CLOSEDBracket;
    const std::string IMPLY;
    const std::string DOUBLEImply;
    const std::string EQuantifier;
    const std::string VQuantifier;

    private:
    std::unordered_map<std::string, std::string> mapping;
    std::unordered_map<std::string, std::string> inv;

    Operators()
    : AND("&"), OR("|"), NOT("~"), OPENEDBracket("("), CLOSEDBracket(")"), IMPLY("->"), DOUBLEImply("<->"),
      EQuantifier("?"), VQuantifier("@") {
        mapping["AND"]         = AND;
        mapping["OR"]          = OR;
        mapping["NOT"]         = NOT;
        mapping["OPENED"]      = OPENEDBracket;
        mapping["CLOSED"]      = CLOSEDBracket;
        mapping["IMPLY"]       = IMPLY;
        mapping["DOUBLEImply"] = DOUBLEImply;
        mapping["E"]           = EQuantifier;
        mapping["@"]           = VQuantifier;
        for(auto& elem : mapping) { inv[elem.second] = elem.first; }
    }

    Operators(Operators const&);

    void operator=(Operators const&);

    public:
    static Operators& getInstance() {
        static Operators instance;
        return instance;
    }

    std::string whichOperator(const int& position, const std::string& seq);

    std::string advanceOperator(int& position, const std::string& seq, const std::string& result);

    bool isQuantifier(const std::string& current);

    bool isQuantifierAndVariable(const std::string& current);

    std::string getQuantifierFromQuantifierAndVariable(const std::string& current);

    std::string getVariableFromQuantifierAndVariable(const std::string& current);

    std::string getOperator(const std::string& which);

    std::string flipQuantifier(const std::string& which);

    bool isNot(const std::string& which);

    bool isAnd(const std::string& which);

    bool isOr(const std::string& which);

    std::string flipAndOrOr(const std::string& which);
};
} // namespace utils

#endif // PROJECT_OPERATORS_H
