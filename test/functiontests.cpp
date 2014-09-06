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
  testList=stadic::split(testString, ',');
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
