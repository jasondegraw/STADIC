/****************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted for
 * personal and commercial purposes provided that the
 * following conditions are met:
 *
 * 1. Redistribution of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistribution in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University
 *
 * THIS SOFTWARE IS PROVIDED BY THE PENNSYLVANIA STATE UNIVERSITY
 * "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************/

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
    std::string nothing = " ";
    EXPECT_TRUE(stadic::trim(nothing).empty());
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
