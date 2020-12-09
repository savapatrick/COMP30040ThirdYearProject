#include "lib/operators.h"
#include "gtest/gtest.h"
#include <string>

using namespace std;


TEST(operatorsTestSuite, testAND) {
    utils::Operators& operators = utils::Operators::getInstance();
    const string seq            = "^patrick";
    int i                       = 0;
    ASSERT_EQ(operators.advanceOperator(i, seq, "AND"), "^");
    ASSERT_EQ(i, 1);
}

TEST(operatorsTestSuite, testOR) {
    utils::Operators& operators = utils::Operators::getInstance();
    const string seq            = "|patrick";
    int i                       = 0;
    ASSERT_EQ(operators.advanceOperator(i, seq, "OR"), "|");
    ASSERT_EQ(i, 1);
}

TEST(operatorsTestSuite, testNOT) {
    utils::Operators& operators = utils::Operators::getInstance();
    const string seq            = "~patrick";
    int i                       = 0;
    ASSERT_EQ(operators.advanceOperator(i, seq, "NOT"), "~");
    ASSERT_EQ(i, 1);
}

TEST(operatorsTestSuite, testOPENED) {
    utils::Operators& operators = utils::Operators::getInstance();
    const string seq            = "(patrick";
    int i                       = 0;
    ASSERT_EQ(operators.advanceOperator(i, seq, "OPENED"), "(");
    ASSERT_EQ(i, 1);
}

TEST(operatorsTestSuite, testCLOSED) {
    utils::Operators& operators = utils::Operators::getInstance();
    const string seq            = ")patrick";
    int i                       = 0;
    ASSERT_EQ(operators.advanceOperator(i, seq, "CLOSED"), ")");
    ASSERT_EQ(i, 1);
}

TEST(operatorsTestSuite, testIMPLY) {
    utils::Operators& operators = utils::Operators::getInstance();
    const string seq            = "->patrick";
    int i                       = 0;
    ASSERT_EQ(operators.advanceOperator(i, seq, "IMPLY"), "->");
    ASSERT_EQ(i, 2);
}

TEST(operatorsTestSuite, testDOUBLEIMPLY) {
    utils::Operators& operators = utils::Operators::getInstance();
    const string seq            = "<->patrick";
    int i                       = 0;
    ASSERT_EQ(operators.advanceOperator(i, seq, "DOUBLEImply"), "<->");
    ASSERT_EQ(i, 3);
}

TEST(operatorsTestSuite, testExistential) {
    utils::Operators& operators = utils::Operators::getInstance();
    const string seq            = "?patrick";
    int i                       = 0;
    ASSERT_EQ(operators.advanceOperator(i, seq, "E"), "?");
    ASSERT_EQ(i, 1);
}

TEST(operatorsTestSuite, testUniversal) {
    utils::Operators& operators = utils::Operators::getInstance();
    const string seq            = "@patrick";
    int i                       = 0;
    ASSERT_EQ(operators.advanceOperator(i, seq, "@"), "@");
    ASSERT_EQ(i, 1);
}

TEST(operatorsTestSuite, testNone) {
    utils::Operators& operators = utils::Operators::getInstance();
    const string seq            = "p<->atrick";
    int i                       = 0;
    ASSERT_THROW(operators.advanceOperator(i, seq, "none"), logic_error);
    ASSERT_EQ(i, 0);
}
