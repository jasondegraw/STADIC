#include "functions.h"
#include "gtest/gtest.h"
#include <string>
#include <vector>

TEST(FunctionTests, Split)
{
  std::string testString = "This,is,only,a,test";
  std::vector<std::string> testList;
  testList.clear();
  testList=stadic::split(testString, ',');
  ASSERT_EQ(5,testList.size());
  EXPECT_EQ("This",testList[0]);
  EXPECT_EQ("is",testList[1]);
  EXPECT_EQ("only",testList[2]);
  EXPECT_EQ("a",testList[3]);
  EXPECT_EQ("test",testList[4]);
}

TEST(FunctionTests, SplitTrim)
{
  std::string testString = "This, is , only,a ,     test";
  std::vector<std::string> testList;
  testList.clear();
  testList=stadic::trimmedSplit(testString, ',');
  ASSERT_EQ(5,testList.size());
  EXPECT_EQ("This",testList[0]);
  EXPECT_EQ("is",testList[1]);
  EXPECT_EQ("only",testList[2]);
  EXPECT_EQ("a",testList[3]);
  EXPECT_EQ("test",testList[4]);
}


TEST(FunctionTests, Trim)
{
    std::string testString = " word   ";
    EXPECT_EQ("word",stadic::trim(testString));
}

TEST(FunctionTests, ToDouble)
{
    std::string string = "20.5";
    bool ok;
    double value = stadic::toDouble(string, &ok);
    EXPECT_EQ(20.5,value);
    EXPECT_TRUE(ok);
    string = "not a number";
    value = stadic::toDouble(string, &ok);
    EXPECT_EQ(0.0, value);
    EXPECT_FALSE(ok);
}

TEST(FunctionTests, ToString)
{
  std::string string = "20.5";
  double value = 20.5;
  EXPECT_EQ(string, stadic::toString(value));
}