//
// Created by Patrick Sava on 11/4/2020.
//

#include "tokenizer.h"
#include "operators.h"
#include <algorithm>
#include <sstream>
#include <variant>

using namespace std;

namespace utils {
std::vector<std::string> Tokenizer::tokenize(const std::string& seq) {
    string aux(seq);
    /// get rid of all of the whitespaces
    aux.erase(remove_if(aux.begin(), aux.end(), [](char c) { return isspace(c); }), aux.end());
    vector<string> tokens;
    Operators& operators = Operators::getInstance();
    int ind              = 0;
    while(ind < (int)aux.size()) {
        string current;
        if(isupper(aux[ind])) {
            do {
                current += aux[ind];
                ind += 1;
            } while(ind < (int)aux.size() and aux[ind] != ')');
            current += aux[ind];
            if (count_if(current.begin(), current.end(), [](unsigned char currentCharacter)
               {return isupper(currentCharacter);}) > 1) {
                throw invalid_argument("Predicates are allowed to start with uppercase letters and contain only uppercase further"
                                       " and variables are lowercase only!");
            }
            ind += 1;
        } else {
            auto result = operators.whichOperator(ind, aux);
            if(result != "none") {
                current += operators.advanceOperator(ind, aux, result);
            } else {
                while(ind < (int)aux.size() and operators.whichOperator(ind, aux) == "none" and !isupper(aux[ind])) {
                    current += aux[ind];
                    ind += 1;
                }
                // this should be a variable
                // a variable could occur at this point ONLY after a quantifier
                if(!islower(current[0]) or !(!tokens.empty() and (operators.isEquality(tokens.back()) or operators.isQuantifier(tokens.back())))) {
                    // this means that this does not start with lowercase letter OR
                    // there are no tokens in the list
                    throw invalid_argument("Something expected to be variable was either starting with non-lowercase letter"
                                           "or occurred after something different than equality or quantifier");
                }
                tokens[(int)tokens.size() - 1] += current;
                continue;
            }
        }
        tokens.emplace_back(current);
    }
    vector <string> resultingTokens;
    for (auto &token : tokens) {
        if (isupper(token.at(0))) {
            if (token == "Equality") {
                throw invalid_argument("The given formula contains a predicate called Equality."
                                       "This is a keyword and cannot be used as a predicate name.");
            }
        }
        if (!resultingTokens.empty() and operators.isEquality(resultingTokens.back())) {
            resultingTokens.pop_back();
            if (!islower(token.at(0)) or resultingTokens.empty() or
            (!resultingTokens.empty() and !islower(resultingTokens.back().at(0)))) {
                throw invalid_argument("The given string violates the grammar for the equality sign");
            }
            auto result = "Equality(" + resultingTokens.back() + "," + token + ")";
            resultingTokens.pop_back();
            resultingTokens.push_back(result);
        }
        else {
            resultingTokens.push_back(token);
        }
    }
    return resultingTokens;
}

std::pair<std::string, std::vector<std::variant<std::string, std::pair<std::string, std::vector<std::string>>>>>
Tokenizer::decomposePredicate(const string& seq) {
    if(find_if(seq.begin(), seq.end(), [](char c) { return isspace(c); }) != seq.end()) {
        throw invalid_argument("the string should not contain whitespaces");
    }
    auto notAtAll    = static_cast<bool>(seq.find('(') == string::npos or seq.find(')') == string::npos);
    auto countOpened = static_cast<bool>(count(seq.begin(), seq.end(), '(') != 1);
    auto countClosed = static_cast<bool>(count(seq.begin(), seq.end(), ')') != 1);
    if(notAtAll or countOpened or countClosed) {
        throw invalid_argument("the predicate should contain exactly one ( and one ) in the given argument");
    }
    auto correctOrder = static_cast<bool>(seq.find('(') < seq.find(')'));
    if(!correctOrder) {
        throw invalid_argument(") occurs before ( in the given argument");
    }
    auto firstBracket      = seq.find('(');
    auto lastBracket       = seq.find(')');
    auto lengthOfArguments = lastBracket - firstBracket - 1;
    auto predicateName     = seq.substr(0, firstBracket);
    auto arguments         = seq.substr(firstBracket + 1, lengthOfArguments);
    stringstream stream(arguments);
    std::vector<std::variant<std::string, std::pair<std::string, std::vector<std::string>>>> argumentsParsed;
    while(stream.good()) {
        string current;
        getline(stream, current, ',');
        argumentsParsed.emplace_back(current);
    }
    return { predicateName, argumentsParsed };
}
}; // namespace utils
