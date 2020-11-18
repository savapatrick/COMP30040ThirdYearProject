#include "lib/tokenizer.h"
#include "gtest/gtest.h"
#include <string>

using namespace std;

TEST(tokenizerTestSuite, testDecomposePredicate) {
    const string predicate = "AreBrothers(x1,y1,x2,y2,x3,y3,x4,y4,z,a,b,c,d)";
    pair<string, std::vector<std::variant<std::string, std::pair<std::string, std::vector<std::string>>>>> answer;
    answer.first                = "AreBrothers";
    answer.second               = { "x1", "y1", "x2", "y2", "x3", "y3", "x4", "y4", "z", "a", "b", "c", "d" };
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    ASSERT_EQ(tokenizer.decomposePredicate(predicate), answer);
}

TEST(tokenizerTestSuite, testDecomposePredicateThrows) {
    const string predicate      = "AreBrothers(x1,y1,x2, y2,x3,y3,x4,y4,z,a,b,c,d)";
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    ASSERT_THROW(tokenizer.decomposePredicate(predicate), invalid_argument);
}

TEST(tokenizerTestSuite, testTokenize) {
    const string formula        = "~   @   x  F (  x )  <   - > ?y G(y) -> @   x P(x)";
    vector<string> result       = { "~", "@x", "F(x)", "<->", "?y", "G(y)", "->", "@x", "P(x)" };
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    ASSERT_EQ(tokenizer.tokenize(formula), result);
}

TEST(tokenizerTestSuite, testTokenizeTwo) {
    const string formula  = "?x(Artst(x)&@y(Bkpr(y)->~Hate(x,y)))";
    vector<string> result = { "?x", "(", "Artst(x)", "&", "@y", "(", "Bkpr(y)", "->", "~", "Hate(x,y)", ")", ")" };
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    ASSERT_EQ(tokenizer.tokenize(formula), result);
}

TEST(tokenizerTestSuite, testTokenizeThree) {
    const string formula        = "@x(Bkpr(x) -> ?y(Artst(y) & Hate(x, y)))";
    vector<string> result       = { "@x", "(", "Bkpr(x)", "->", "?y", "(", "Artst(y)", "&", "Hate(x,y)", ")", ")" };
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    ASSERT_EQ(tokenizer.tokenize(formula), result);
}

TEST(tokenizerTestSuite, testTokenizeFour) {
    const string formula  = "@x1@x2@x3~~~~?y1?y2?y3(P(x1, y1, y2) < - > Q(x2, y3, x3))";
    vector<string> result = { "@x1", "@x2", "@x3", "~", "~", "~", "~", "?y1", "?y2", "?y3", "(", "P(x1,y1,y2)", "<->",
        "Q(x2,y3,x3)", ")" };
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    ASSERT_EQ(tokenizer.tokenize(formula), result);
}

TEST(tokenizerTestSuite, testTokenizeFive) {
    const string formula  = "@x1@x2@x3(P(x1)->Q(x2)->R(x3)<->?x4?x5T(x1, x2, x3, x4, x5))";
    vector<string> result = { "@x1", "@x2", "@x3", "(", "P(x1)", "->", "Q(x2)", "->", "R(x3)", "<->", "?x4", "?x5",
        "T(x1,x2,x3,x4,x5)", ")" };
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    ASSERT_EQ(tokenizer.tokenize(formula), result);
}
