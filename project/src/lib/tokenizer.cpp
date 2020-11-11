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
        /// TODO: when introducing equality, manually remove all of the equality signs and replace them
        /// with an equality predicate of arity 2, possibly with a special name
        string aux(seq);
        /// get rid of all of the whitespaces
        aux.erase(remove_if(aux.begin(), aux.end(), [](char c) {return isspace(c);}), aux.end());
        /// FROM NOW, each predicate starts with UPPER-CASE letter
        /// functions can occur only as an argument to a predicate
        /// both functions and variables start with lower-case letters
        /// TODO: reconsider whether this is okay or whether we really want lower-case starting predicates
        vector <string> tokens;
        Operators& operators = Operators::getInstance();
        int ind = 0;
        while (ind < (int)aux.size()) {
            string current;
            if (isupper(aux[ind])) {
                do {
                    current += aux[ind];
                    ind += 1;
                }while(ind < (int)aux.size() and aux[ind] != ')');
                current += aux[ind];
                ind += 1;
            }
            else {
                auto result = operators.whichOperator(ind, aux);
                if (result != "none") {
                    current += operators.advanceOperator(ind, aux, result);
                }
                else {
                    while (ind < (int) aux.size() and operators.whichOperator(ind, aux) == "none" and !isupper(aux[ind])) {
                        current += aux[ind];
                        ind += 1;
                    }
                    // this should be a variable
                    // a variable could occur at this point ONLY after a quantifier
                    if (!islower(current[0]) or !(!tokens.empty() and operators.isQuantifier(tokens.back()))) {
                        // this means that this does not start with lowercase letter OR
                        // there are no tokens in the list
                        throw invalid_argument("the given formula is malformed");
                    }
                    tokens[(int)tokens.size() - 1] += current;
                    continue;
                }
            }
            tokens.emplace_back(current);
        }
        // TODO: Incredibly important, enforce the order of the tokens to be initial one
        return tokens;
    }

    std::pair<std::string, std::vector<std::string> > Tokenizer::decomposePredicate(const string &seq) {
        if (find_if(seq.begin(), seq.end(), [](char c){return isspace(c);}) != seq.end()) {
            throw invalid_argument("the string should not contain whitespaces");
        }
        auto notAtAll = static_cast<bool>(seq.find('(') == string::npos or seq.find(')') == string::npos);
        auto countOpened = static_cast<bool>(count(seq.begin(), seq.end(), '(') != 1);
        auto countClosed = static_cast<bool>(count(seq.begin(), seq.end(), ')') != 1);
        if (notAtAll or countOpened or countClosed) {
            throw invalid_argument("the predicate should contain exactly one ( and one ) in the given argument");
        }
        auto correctOrder = static_cast<bool>(seq.find('(') < seq.find(')'));
        if (!correctOrder) {
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

    std::pair<std::string, std::string> Tokenizer::decomposeQuantifierAndVariable(const string &seq) {
        Operators& operators = Operators::getInstance();
        if (!operators.isQuantifierAndVariable(seq)) {
            throw invalid_argument("expected quantifier and variable but given " + seq);
        }
        string quantifier = seq.substr(0, 1);
        string variable = seq.substr(1);
        if (std::find_if_not(variable.begin(), variable.end(),
                             [](char c){return islower(c);}) != variable.end()){
            throw invalid_argument("expected exactly one variable but given " + variable);
        }
        return {quantifier, variable};
    }
};
