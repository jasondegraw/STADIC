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
 *    and the following disclaimer.
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University.
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

#include "filepath.h"
#include "functions.h"
#include "gtest/gtest.h"
#include <string>
#include <fstream>


#ifdef _MSC_VER
#include <windows.h>            //Added for Sleep(ms)
#define UNLINK _unlink
#else
#include <unistd.h>             //Added for sleep(s)
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
    stadic::FilePath dir(testString);
    EXPECT_TRUE(dir.exists());
    EXPECT_TRUE(dir.isDir());
    EXPECT_FALSE(dir.isFile());
    EXPECT_FALSE(dir.isUpdated());

#ifdef _WIN32
    testString = "C:\\Windows\\";
#else
    testString = "/usr/";
#endif
    stadic::FilePath dir1(testString);
    EXPECT_TRUE(dir1.exists());
    EXPECT_TRUE(dir1.isDir());
    EXPECT_FALSE(dir1.isFile());
    EXPECT_FALSE(dir1.isUpdated());

    stadic::FilePath dir2("DOESNOTEXIST");
    EXPECT_FALSE(dir2.exists());
}

TEST(FilePathTests, File)
{
    std::string testString = "testfile.txt";
    UNLINK(testString.c_str());

    stadic::FilePath file(testString);
    EXPECT_FALSE(file.exists());

    std::ofstream testOut(testString);
    testOut << "This is a test file";
    testOut.close();

    EXPECT_TRUE(file.exists());
    EXPECT_TRUE(file.isFile());
    EXPECT_FALSE(file.isDir());
    EXPECT_FALSE(file.isUpdated());
#ifdef _MSC_VER
    Sleep(1000);            //This is to delay for one second (the units are in milliseconds)
#else
    sleep(1);               //This is to delay for one second
#endif
    //There may need to be a delay inserted here so the updated time actually changes
    std::ofstream reWrite(testString);
    reWrite << "I'm doing this as hard as I can";
    reWrite.close();

    EXPECT_TRUE(file.exists());
    EXPECT_TRUE(file.isFile());
    EXPECT_FALSE(file.isDir());
    EXPECT_TRUE(file.isUpdated());

}
