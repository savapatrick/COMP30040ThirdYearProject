//
// Created by Patrick Sava on 11/4/2020.
//

#include "lib/parse_tree.h"
#include "lib/tokenizer.h"
#include "lib/basic_theorem_prover.h"
#include "lib/simplified_clause_form.h"
#include <fstream>

// C++ 17, because it uses variant

using namespace std;

int main() {
    ifstream input("input.txt");
    ofstream output("output.txt");
    string formula;
    vector <std::shared_ptr<utils::SimplifiedClauseForm>> simplifiedClauseForms;
    while(getline(input, formula)) {
        output << "for the formula " + formula << "\nthe clause form is\n";
        utils::ParseTree tree(formula);
        utils::Reducer reducer(tree);
        output << "clause form is " << reducer.getSimplifiedClauseForm<string>() << '\n';
        output << "\n";
        auto simplifiedClauseForm = reducer.getSimplifiedClauseForm<std::shared_ptr<utils::SimplifiedClauseForm>>();
        simplifiedClauseForms.emplace_back(simplifiedClauseForm);
    }
    /* TODO : aggregate simplifiedClauseForms in clauseForm
     * TODO: find a smart way of negating last formula --- assuming that the last formula is the one to be proved */
    std::shared_ptr<utils::ClauseForm> clauseForm;
    utils::BasicTheoremProver basicTheoremProver(clauseForm, "basic_theorem_prover_output.txt");
    basicTheoremProver.run();
    return 0;
}
