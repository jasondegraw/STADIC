/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
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

#include "filepath.h"
#include "functions.h"
#include "gtest/gtest.h"
#include <string>
#include <fstream>


#ifdef _MSC_VER
//#include <windows.h>            //Added for Sleep(ms)
#define UNLINK _unlink
#else
//#include <unistd.h>             //Added for sleep(s)
#define UNLINK unlink
#endif

TEST(FilePathTests, Directory)
{
    std::string testString;
#ifdef _WIN32
    testString = "C:\\Windows";
#else
    testString = "/usr";
#endif
    EXPECT_TRUE(stadic::exists(testString));
    EXPECT_TRUE(stadic::isDir(testString));
    EXPECT_FALSE(stadic::isFile(testString));

#ifdef _WIN32
    testString = "C:\\Windows\\";
#else
    testString = "/usr/";
#endif
    EXPECT_TRUE(stadic::exists(testString));
    EXPECT_TRUE(stadic::isDir(testString));
    EXPECT_FALSE(stadic::isFile(testString));

    testString = "HOPEFULLYNOBODYWOULDNAMEADIRECTORYTHIS";
    EXPECT_FALSE(stadic::exists(testString));
    EXPECT_FALSE(stadic::isDir(testString));
    EXPECT_FALSE(stadic::isFile(testString));
}

TEST(FilePathTests, File)
{
    std::string testString = "testfile.txt";
    UNLINK(testString.c_str());

    std::ofstream testOut(testString);
    testOut << "This is a test file";
    testOut.close();

    EXPECT_TRUE(stadic::exists(testString));
    EXPECT_TRUE(stadic::isFile(testString));
    EXPECT_FALSE(stadic::isDir(testString));
    UNLINK(testString.c_str());
}

TEST(FilePathTests, PathNameDir)
{
    std::string pathstring("this/is/a/subdir/");
    stadic::PathName path(pathstring);
    ASSERT_TRUE(path.create());
    std::string filestring("this/is/a/file");
    stadic::PathName file(filestring);
    ASSERT_TRUE(file.create());
#ifdef _WIN32
    std::string checkpath("this\\is\\a\\subdir");
    std::string checkfile("this\\is\\a\\file");
#else
    std::string checkpath("this/is/a/subdir");
    std::string checkfile("this/is/a/file");
#endif
    ASSERT_TRUE(stadic::exists(checkpath));
    ASSERT_TRUE(stadic::exists(checkfile));
}
