//
// Created by Patrick Sava on 11/4/2020.
//

#include "operators.h"

using namespace std;

namespace utils {
    string Operators::whichOperator(int position, const std::string& seq) {
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
        }
        return "none";
    }

    void Operators::advanceOperator(int &position, const string &seq, const string &result) {
        if (mapping.find(result) != mapping.end()) {
            position += mapping[result].size();
        }
        else {
            if(result == "none") {
                return;
            }
            throw out_of_range("result of the operator was altered");
        }
    }

    bool Operators::isQuantifier(const string &current) {
        return current == EQuantifier or current == VQuantifier;
    }
}
