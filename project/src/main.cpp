//
// Created by Patrick Sava on 11/4/2020.
//

#include "lib/basic_theorem_prover.h"
#include "lib/parse_tree.h"
#include "lib/tokenizer.h"
#include <fstream>
#include <lib/depth_ordered_theorem_prover.h>
#include <lib/two_variable_theorem_prover.h>
#include <memory>

// C++ 17, because it uses variant

using namespace std;

int main(int argc, char* argv[]) {
    int commandMask = 0;
    map<string, int> whichMask({ { "none", 0 }, { "all", 7 }, { "depth", 4 }, { "basic", 2 }, { "two", 1 } });
    map<string, int> powerCommand({ { "depth", 2 }, { "basic", 1 }, { "two", 0 } });
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
    utils::ParseTree tree(formulas);
    vector<std::shared_ptr<utils::ClauseForm>> clauseForms;
    for(int index = 0; index < (int)formulas.size(); ++index) {
        auto formula = formulas[index];
        if(index + 1 == (int)formulas.size()) {
            output << "the formula " + formula << " is the one to be proved\n";
        } else {
            output << "the formula " + formula << " is part of the knowledge base\n";
        }
        if(index + 1 == (int)formulas.size()) {
            output << "so it has to be negated to ~(" + formula + ")\n";
        }
    }
    utils::Reducer reducer(tree);
    output << "The clause form for the given set of formulas is " << reducer.getSimplifiedClauseForm<string>() << '\n';
    output.flush();
    auto clauseForm = reducer.getClauseForm();
    output << clauseForm->getString() << '\n';
    output.flush();
    if(commandMask & (1 << powerCommand["basic"])) {
        utils::BasicTheoremProver basicTheoremProver(clauseForm, false, "basic_theorem_prover_output.txt");
        basicTheoremProver.run();
    }
    if(commandMask & (1 << powerCommand["depth"])) {
        utils::DepthOrderedTheoremProver depthOrderedTheoremProver(clauseForm, false,"depth_ordered_theorem_prover_output.txt");
        depthOrderedTheoremProver.run();
    }
    if(commandMask & (1 << powerCommand["two"])) {
        utils::TwoVariableTheoremProver twoVariableTheoremProver(clauseForm, true, "two_variable_theorem_prover_output.txt");
        twoVariableTheoremProver.run();
    }
    return 0;
}
