//
// Created by Patrick Sava on 11/4/2020.
//

#include "lib/parse_tree.h"
#include "lib/tokenizer.h"
#include <iostream>

// C++ 17, because it uses variant

using namespace std;

int main() {
    string formula;
    while(getline(cin, formula)) {
        cout << "for the formula " + formula << "\nthe clause form is\n";
        utils::ParseTree tree(formula);
        utils::Reducer reducer(tree);
        cout << reducer.getClauseForm() << '\n';
        cout << "\n";
    }
    return 0;
}
