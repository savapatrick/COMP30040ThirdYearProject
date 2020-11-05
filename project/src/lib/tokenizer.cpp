//
// Created by Patrick Sava on 11/4/2020.
//

#include "tokenizer.h"
#include "operators.h"
#include <algorithm>
#include <sstream>

using namespace std;

namespace utils {
    std::vector<std::string> Tokenizer::tokenize(const std::string &seq) const {
        string aux(seq);
        /// get rid of all of the whitespaces
        aux.erase(remove_if(aux.begin(), aux.end(), [](char c) {return isspace(c);}), aux.end());
        /// FROM NOW, each predicate starts with UPPER-CASE letter
        /// functions can occur only as an argument to a predicate
        /// both functions and variables start with lower-case letters
        /// TODO: reconsider whether this is okay or whether we really want lower-case starting predicates
        vector <string> tokens;
        Operators& operators = Operators::getInstance();
        for (int ind = 0; ind < (int)aux.size(); ++ ind) {
            string current;
            if (isupper(aux[ind])) {
                do {
                    current += aux[ind];
                    ind += 1;
                }while(ind < (int)aux.size() and aux[ind] != ')');
            }
            else {
                while (ind < (int) aux.size() and operators.whichOperator(ind, aux) == "none" and !isupper(aux[ind])) {
                    current += aux[ind];
                    ind += 1;
                }
                if (ind < (int)aux.size() and aux[ind] == '>') {
                    // edge case for ->
                    current += aux[ind];
                    ind += 1;
                }
            }
            if (islower(aux[ind]) and !tokens.empty() and operators.isQuantifier(tokens.back())) {
                // we have a variable and before it was quantifier
                current = tokens.back() + current;
                tokens.pop_back();
            }
            tokens.emplace_back(current);
        }
        // TODO: Incredibly important, enforce the order of the tokens to be initial one
        return tokens;
    }

    std::pair<std::string, std::vector<std::string> > Tokenizer::decomposePredicate(const string &seq) {
        auto notAtAll = static_cast<bool>(seq.find('(') == string::npos or seq.find(')') == string::npos);
        auto countOpened = static_cast<bool>(count(seq.begin(), seq.end(), '(') != 1);
        auto countClosed = static_cast<bool>(count(seq.begin(), seq.end(), ')') != 1);
        if (notAtAll or countOpened or countClosed) {
            throw invalid_argument("the predicate should contain exactly one ( and one ) in the given argument");
        }
        auto correctOrder = static_cast<bool>(seq.find('(') < seq.find(')'));
        if (correctOrder) {
            throw invalid_argument(") occurs before ( in the given argument");
        }
        auto firstBracket = seq.find('(');
        auto lastBracket = seq.find(')');
        auto lengthOfArguments = lastBracket - firstBracket - 1;
        auto predicateName = seq.substr(0, firstBracket);
        auto arguments = seq.substr(firstBracket + 1, lengthOfArguments);
        stringstream stream(arguments);
        vector <string> argumentsParsed;
        while(stream.good()) {
            string current;
            getline(stream, current, ',');
            argumentsParsed.push_back(current);
        }
        return {predicateName, argumentsParsed};
    }
};
