//
// Created by Patrick Sava on 11/4/2020.
//

#include "lib/basic_theorem_prover.h"
#include "lib/parse_tree.h"
#include "lib/tokenizer.h"
#include <fstream>
#include <lib/two_variable_theorem_prover.h>
#include <memory>

// C++ 17, because it uses variant

using namespace std;

int main(int argc, char* argv[]) {
    int commandMask = 0;
    map<string, int> whichMask({ { "none", 4 }, { "all", 3 }, { "basic", 2 }, { "two", 1 } });
    map<string, int> powerCommand({ { "basic", 1 }, { "two", 0 } });
    if(argc > 1) {
        string command(argv[1]);
        commandMask = whichMask[command];
    } else {
        throw std::invalid_argument("No arguments were provided!\n");
    }
    ifstream input("input.txt");
    ofstream output("clause_form.txt");
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
    if(commandMask & (1 << powerCommand["basic"])) {
        utils::BasicTheoremProver basicTheoremProver(clauseForm, "basic_theorem_prover_output.txt");
        basicTheoremProver.run();
    }
    if(commandMask & (1 << powerCommand["two"])) {
        utils::TwoVariableTheoremProver twoVariableTheoremProver(clauseForm, "two_variable_theorem_prover_output.txt");
        twoVariableTheoremProver.run();
    }
    return 0;
}
