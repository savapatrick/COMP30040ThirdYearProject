#include "gtest/gtest.h"
#include "lib/parse_tree.h"
#include "lib/tokenizer.h"

TEST(parseTreeTestSuite, testBuildTreeScenarioOne) {
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    const std::string formula = "@y?x~@zF(x,y,z)->@x@y(P(x,y)<->R(y)&T(x))";
    std::vector <std::string> tokens = tokenizer.tokenize(formula);
    auto parseTree = utils::ParseTree(tokens);
    ASSERT_EQ(parseTree.getEulerTraversal(), "");
}

TEST(parseTreeTestSuite, testBuildTreeScenarioTwo) {

}

TEST(parseTreeTestSuite, testBuildTreeScenarioThree) {

}

TEST(parseTreeTestSuite, testBuildTreeScenarioFour) {

}

TEST(parseTreeTestSuite, testBuildTreeScenarioFive) {

}