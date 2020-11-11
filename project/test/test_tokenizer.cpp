#include "gtest/gtest.h"
#include "lib/tokenizer.h"
#include <string>

using namespace std;

TEST(tokenizerTestSuite, testDecomposePredicate) {
    const string predicate = "AreBrothers(x1,y1,x2,y2,x3,y3,x4,y4,z,a,b,c,d)";
    pair <string, vector <string>> answer;
    answer.first = "AreBrothers";
    answer.second = {"x1", "y1", "x2", "y2", "x3", "y3", "x4", "y4", "z", "a", "b", "c", "d"};
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    ASSERT_EQ(tokenizer.decomposePredicate(predicate), answer);
}

TEST(tokenizerTestSuite, testDecomposePredicateThrows) {
    const string predicate = "AreBrothers(x1,y1,x2, y2,x3,y3,x4,y4,z,a,b,c,d)";
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    ASSERT_THROW(tokenizer.decomposePredicate(predicate), invalid_argument);
}

