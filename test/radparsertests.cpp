/******************************************************************************
 * Copyright (c) 2015, Jason W. DeGraw
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission of the
 *    respective copyright holder or contributor.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * AND NONINFRINGEMENT OF INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN
 * NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *****************************************************************************/

#include "radparser.h"
#include "gtest/gtest.h"
#include <string>
#include <sstream>
#include <vector>
#include <iostream>

TEST(RadParserTests, Empty)
{
    std::stringstream stream;
    stadic::RadParser parser(stream);
    boost::optional<std::string> next;
    std::vector<std::string> strings;
    while(next = parser.next()) {
        strings.push_back(next.get());
    }
    ASSERT_EQ(0, strings.size());
    EXPECT_EQ(1, parser.linenumber());
}

TEST(RadParserTests, Basic)
{
    std::string rad("void plastic l_wall\n0\n0\n5 .5 .5 .5 0 0");
    std::stringstream stream;
    stream << rad;
    stadic::RadParser parser(stream);
    boost::optional<std::string> next;
    std::vector<std::string> strings;
    while(next = parser.next()) {
        //std::cout << "Next string: " << next.get() << std::endl;
        strings.push_back(next.get());
    }
    ASSERT_EQ(11, strings.size());
    EXPECT_EQ(4, parser.linenumber());
}

TEST(RadParserTests, OneLiner)
{
    std::string rad("void plastic l_wall 0 0 5 .5 .5 .5 0 0");
    std::stringstream stream;
    stream << rad;
    stadic::RadParser parser(stream);
    boost::optional<std::string> next;
    std::vector<std::string> strings;
    while(next = parser.next()) {
        //std::cout << "Next string: " << next.get() << std::endl;
        strings.push_back(next.get());
    }
    ASSERT_EQ(11, strings.size());
    EXPECT_EQ(1, parser.linenumber());
}

TEST(RadParserTests, Commented)
{
    std::string rad("void plastic l_wall # This is a comment\n0\n     # A comment with leading spaces\n0\n# Yet another comment\n5 .5 .5 .5 0 0");
    std::stringstream stream;
    stream << rad;
    stadic::RadParser parser(stream);
    boost::optional<std::string> next;
    std::vector<std::string> strings;
    while(next = parser.next()) {
        //std::cout << "Next string: " << next.get() << std::endl;
        strings.push_back(next.get());
    }
    ASSERT_EQ(11, strings.size());
    EXPECT_EQ(6, parser.linenumber());
}
