#include "gtest/gtest.h"
#include "lib/parse_tree.h"
#include "lib/tokenizer.h"

TEST(parseTreeTestSuite, testBuildTreeScenarioOne) {
    const std::string formula = "@y?x~@zF(x,y,z)->@x@y(P(x,y)<->R(y)&T(x))";
    auto parseTree = utils::ParseTree(formula);
    const std::string eulerExpected =
            "$none$@y?x~@zF(x,y,z)F(x,y,z)@z~?x@y->->@x@y$none$P(x,y)P(x,y)<-><->R(y)R(y)&&T(x)T(x)$none$@y@x$none$";
    ASSERT_EQ(parseTree.getEulerTraversal(), eulerExpected);
}

TEST(parseTreeTestSuite, testBuildTreeScenarioTwo) {
    const std::string formula = "@x?y?z(R(x,y,z)<->P(x)&T(x,y)|W(x,z)->Q(z))";
    auto parseTree = utils::ParseTree(formula);
    const std::string eulerExpected =
            "$none$@x?y?z$none$R(x,y,z)R(x,y,z)<-><->P(x)P(x)&&T(x,y)T(x,y)||W(x,z)W(x,z)->->Q(z)Q(z)$none$?z?y@x$none$";
    ASSERT_EQ(parseTree.getEulerTraversal(), eulerExpected);
}

TEST(parseTreeTestSuite, testBuildTreeScenarioThree) {

}

TEST(parseTreeTestSuite, testBuildTreeScenarioFour) {

}

TEST(parseTreeTestSuite, testBuildTreeScenarioFive) {

}