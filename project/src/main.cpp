//
// Created by Patrick Sava on 11/4/2020.
//

#include "lib/basic_theorem_prover.h"
#include "lib/parse_tree.h"
#include "lib/tokenizer.h"
#include <fstream>
#include <memory>

// C++ 17, because it uses variant

using namespace std;

int main() {
    ifstream input("input.txt");
    ofstream output("output.txt");
    vector<string> formulas;
    string auxFormula;
    while(getline(input, auxFormula)) { formulas.push_back(auxFormula); }
    vector<std::shared_ptr<utils::ClauseForm>> clauseForms;
    for(int index = 0; index < (int)formulas.size(); ++index) {
        auto formula = formulas[index];
        if(index + 1 == (int)formulas.size()) {
            output << "the formula " + formula << " is the one to be proved\n";
        } else {
            output << "the formula " + formula << " is part of the knowledge base\n";
        }
        utils::ParseTree tree(formula);
        utils::Reducer reducer(tree);
        if(index + 1 == (int)formulas.size()) {
            output << "so it has to be negated to ~(" + formula + ")\n";
            reducer.addNegationToRoot();
        }
        output << "clause form for it is " << reducer.getSimplifiedClauseForm<string>() << '\n';
        output.flush();
        auto clauseForm = reducer.getClauseForm();
        clauseForms.emplace_back(clauseForm);
    }
    std::shared_ptr<utils::ClauseForm> clauseForm = make_shared<utils::ClauseForm>();
    for(auto& _clauseForm : clauseForms) { clauseForm->merge(_clauseForm); }
    output << clauseForm->getString() << '\n';
    output.flush();
    utils::BasicTheoremProver basicTheoremProver(clauseForm, "basic_theorem_prover_output.txt");
    basicTheoremProver.run();
    return 0;
}
