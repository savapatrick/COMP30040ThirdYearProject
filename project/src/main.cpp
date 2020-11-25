//
// Created by Patrick Sava on 11/4/2020.
//

#include "lib/parse_tree.h"
#include "lib/tokenizer.h"
#include <fstream>

// C++ 17, because it uses variant

using namespace std;

int main() {
    ifstream input("input.txt");
    ofstream output("output.txt");
    string formula;
    while(getline(input, formula)) {
        output << "for the formula " + formula << "\nthe clause form is\n";
        utils::ParseTree tree(formula);
        utils::Reducer reducer(tree);
        output << "clause form is " << reducer.getClauseForm<string>() << '\n';
        output << "\n";

    }
    return 0;
}
