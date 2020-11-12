//
// Created by Patrick Sava on 11/4/2020.
//

#include <iostream>
#include "lib/tokenizer.h"
#include "lib/parse_tree.h"

// C++ 17, because it uses variant

using namespace std;

int main() {
    string formula;
    while(getline(cin, formula)) {
        cout << "for the formula " + formula << "\nthe clause form is\n";
        utils::ParseTree tree(formula);
        cout << tree.extractClauseForm() << '\n';
        cout << "\n";
    }
    return 0;
}
