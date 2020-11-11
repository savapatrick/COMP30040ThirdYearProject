#include "gtest/gtest.h"
#include "lib/parse_tree.h"
#include "lib/tokenizer.h"

TEST(parseTreeTestSuite, testBuildTreeScenarioOne) {
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    const std::string formula = "@y?x~@zF(x,y,z)->@x@y(P(x,y)<->R(y)&T(x))";
    std::vector <std::string> tokens = tokenizer.tokenize(formula);
    auto parseTree = utils::ParseTree(tokens);
    const std::string eulerExpected =
            "$none$@y?x~@zF(x,y,z)F(x,y,z)@z~?x@y->->@x@y$none$P(x,y)P(x,y)<-><->R(y)R(y)&&T(x)T(x)$none$@y@x$none$";
    ASSERT_EQ(parseTree.getEulerTraversal(), eulerExpected);
}

TEST(parseTreeTestSuite, testBuildTreeScenarioTwo) {
    utils::Tokenizer& tokenizer = utils::Tokenizer::getInstance();
    const std::string formula = "@x?y?z(R(x,y,z)<->P(x)&T(x,y)|W(x,z)->Q(z))";
    std::vector <std::string> tokens = tokenizer.tokenize(formula);
    auto parseTree = utils::ParseTree(tokens);
    const std::string eulerExpected = "";
    ASSERT_EQ(parseTree.getEulerTraversal(), eulerExpected);
}

TEST(parseTreeTestSuite, testBuildTreeScenarioThree) {

}

TEST(parseTreeTestSuite, testBuildTreeScenarioFour) {

}

TEST(parseTreeTestSuite, testBuildTreeScenarioFive) {

}