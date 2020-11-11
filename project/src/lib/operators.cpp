//
// Created by Patrick Sava on 11/4/2020.
//

#include "operators.h"

using namespace std;

namespace utils {
    string Operators::whichOperator(const int& position, const std::string& seq) {
        if(position < (int)seq.size()) {
            throw out_of_range("position beyond end of seq");
        }
        string aux = ""; aux += seq[position];
        if (inv.find(aux) != inv.end()) {
            return inv[aux];
        }
        if (position + 1 < (int)seq.size()) {
            aux += seq[position + 1];
            if (inv.find(aux) != inv.end()) {
                return inv[aux];
            }
            if (position + 2 < (int)seq.size()) {
                aux += seq[position + 2];
                if (inv.find(aux) != inv.end()) {
                    return inv[aux];
                }
            }
        }
        return "none";
    }

    string Operators::advanceOperator(int &position, const string &seq, const string &result) {
        string answer;
        if (mapping.find(result) != mapping.end()) {
            for (int ind = position; ind < position + mapping[result].size(); ++ ind) {
                answer += seq[ind];
            }
            position += mapping[result].size();
        }
        else {
            if(result == "none") {
                throw logic_error("advanceOperator should always return an operator");
            }
            throw out_of_range("result of the operator was altered");
        }
        return answer;
    }

    bool Operators::isQuantifier(const string &current) {
        return current == EQuantifier or current == VQuantifier;
    }
}
