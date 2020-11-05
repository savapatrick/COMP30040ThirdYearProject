//
// Created by Patrick Sava on 11/4/2020.
//

#include "parse_tree.h"

using namespace std;

namespace utils {
    ParseTree::ParseTree(const std::vector<std::string> &tokens) {
        Root = 1;
        buildTree(tokens);
        applyParanthesesToConjunctions(Root);
    }

    void ParseTree::buildTree(const vector<std::string> &tokens) {

    }

    void ParseTree::applyParanthesesToConjunctions(int node) {

    }
}

