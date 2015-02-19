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

#include "stadicprocess.h"
#include "functions.h"
#include "gtest/gtest.h"
#include <string>
#include <fstream>
#include <algorithm>
#ifdef _WIN32
#include <Windows.h>
#else
#include <time.h>
#endif
#ifdef _MSC_VER
#define UNLINK _unlink
#else
#define UNLINK unlink
#endif

std::string readFileToString(const std::string &filename)
{
    std::ifstream stream(filename);
    std::string string((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
    string.erase(std::remove_if(string.begin(), string.end(), ::iscntrl), string.end());
    stream.close();
    return stadic::trim(string);
}

TEST(ProcessTests, ProcessBadProgram)
{
    stadic::Process proc("ThisIsHopefullyNotAProgramName");
    proc.start();
    EXPECT_FALSE(proc.wait());
}

TEST(ProcessTests, ProcessProgram)
{
#ifdef _WIN32
    stadic::Process proc("ipconfig");
#else
    stadic::Process proc("uname");
#endif
    proc.start();
    EXPECT_TRUE(proc.wait());
    //EXPECT_TRUE(proc.output().size() > 0);
}

TEST(ProcessTests, ProcessProgramArgs)
{
    std::vector<std::string> days = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
#ifdef _WIN32
    SYSTEMTIME time;
    GetLocalTime(&time);
    std::string datestring = days[time.wDayOfWeek] + " " + stadic::toString(time.wMonth) + "/"
      + stadic::toString(time.wDay) + "/" + stadic::toString(time.wYear);
    std::vector<std::string> args;
    args.push_back("/c");
    args.push_back("date");
    args.push_back("/t");
    stadic::Process proc("cmd", args);
#else
    std::vector<std::string> args;
    args.push_back("+\"%a %F\"");
    stadic::Process proc("date", args);
    time_t result = time(NULL);
    struct tm *current = localtime(&result);
    std::string datestring = days[current->tm_wday] + " " + stadic::toString(current->tm_year+1900) + "-"
            + stadic::toString(current->tm_mon+1) + "-" + stadic::toString(current->tm_mday);
#endif
    proc.setStandardOutputFile("output.txt");
    proc.start();
    EXPECT_TRUE(proc.wait());
    std::string output = readFileToString("output.txt");
    // std::string output =  proc.output();
    EXPECT_EQ(datestring, stadic::trim(output));
    UNLINK("output.txt");
}

#ifdef _WIN32
#define PROGRAM "testprogram"
#else
#define PROGRAM "./testprogram"
#endif
TEST(ProcessTests, ProcessFileOutErr)
{
    stadic::Process proc(PROGRAM);
    proc.setStandardErrorFile("error.txt");
    proc.setStandardOutputFile("output.txt");
    proc.start();
    ASSERT_TRUE(proc.wait());
    std::string output = readFileToString("output.txt");
    std::string error = readFileToString("error.txt");
    EXPECT_EQ("This is the standard output", output);
    EXPECT_EQ("This is the standard error", error);
    UNLINK("output.txt");
    UNLINK("error.txt");
}

/*
TEST(ProcessTests, ProcessCaptureOutErr)
{
    stadic::Process proc(PROGRAM);
    proc.setStandardErrorFile("error.txt");
    proc.setStandardOutputFile("output.txt");
    proc.start();
    ASSERT_TRUE(proc.wait());
    std::string output = stadic::trim(proc.output());
    std::string error = stadic::trim(proc.error());
    EXPECT_EQ("This is the standard output", output);
    EXPECT_EQ("This is the standard error", error);
    UNLINK("output.txt");
    UNLINK("error.txt");
}
*/

TEST(ProcessTests, ProcessCaptureBigOut)
{
    std::vector<std::string> args;
    args.push_back("-B");
    stadic::Process proc(PROGRAM, args);
    proc.setStandardOutputFile("output.txt");
    proc.start();
    ASSERT_TRUE(proc.wait());
    std::string output = readFileToString("output.txt");
    // std::string output = stadic::trim(proc.output());
    EXPECT_EQ(10004,output.size());
    UNLINK("output.txt");
}

TEST(ProcessTests, ProcessPipeBigOut)
{
    std::vector<std::string> args;
    args.push_back("-B");
    stadic::Process proc0(PROGRAM, args);
    args.clear();
    args.push_back("-d");
    stadic::Process proc1(PROGRAM, args);
    proc0.setStandardOutputProcess(&proc1);
    proc1.setStandardOutputFile("output.txt");
    proc0.start();
    proc1.start();
    ASSERT_TRUE(proc0.wait());
    ASSERT_TRUE(proc1.wait());
    std::string output = readFileToString("output.txt");
    // std::string output = stadic::trim(proc.output());
    EXPECT_EQ(0,output.size());
    UNLINK("output.txt");
}

TEST(ProcessTests, ProcessArgsOutErrFiles)
{
    std::stringstream stream;
    std::vector<std::string> args;
    args.push_back("-x");
    stadic::Process proc(PROGRAM,args);
    proc.setStandardErrorFile("error.txt");
    proc.setStandardOutputFile("output.txt");
    proc.start();
    ASSERT_TRUE(proc.wait());
    std::string outputString = readFileToString("output.txt");
    std::string errorString = readFileToString("error.txt");
    EXPECT_EQ("Here are some standard output letters: xxxxxxxxx", outputString);
    EXPECT_EQ("Here are some standard error letters: xxxxxxxxx", errorString);
    UNLINK("output.txt");
    UNLINK("error.txt");
}

TEST(ProcessTests, ProcessInputFile)
{
    std::ofstream out("input.txt");
    out << "This is line 1" << std::endl;
    out << "This is line 2" << std::endl;
    out << "STOP" << std::endl;
    out.close();

    std::vector<std::string> args;
    args.push_back("-r");
    stadic::Process proc(PROGRAM, args);
    proc.setStandardInputFile("input.txt");
    proc.setStandardOutputFile("output.txt");
    proc.start();
    ASSERT_TRUE(proc.wait());
    std::string output = readFileToString("output.txt");
    //std::string output = stadic::trim(proc.output());
    // Zap control chars...
    //output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:This is line 1Input:This is line 2Input:STOP", output);
    //EXPECT_TRUE(stadic::trim(proc.error()).empty());
    UNLINK("output.txt");
    UNLINK("input.txt");
}

TEST(ProcessTests, ProcessTwoProc)
{
    std::ofstream out("input.txt");
    out << "This is the first line" << std::endl;
    out << "There's not much more" << std::endl;
    out << "STOP" << std::endl;
    out.close();
    
    std::vector<std::string> args0;
    args0.push_back("-r");
    stadic::Process proc0(PROGRAM, args0);
    proc0.setStandardInputFile("input.txt");
    
    std::vector<std::string> args1;
    args1.push_back("-r");
    stadic::Process proc1(PROGRAM, args1);
    proc1.setStandardOutputFile("output.txt");
    proc0.setStandardOutputProcess(&proc1);

    ASSERT_FALSE(proc0.wait());
    ASSERT_FALSE(proc1.wait());

    proc0.start();
    ASSERT_TRUE(proc0.wait());
    proc1.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_TRUE(proc1.wait());

    std::string output = readFileToString("output.txt");
    // std::string output = stadic::trim(proc.output());
    // Zap control chars...
    // output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:This is the first lineInput:Input:There's not much moreInput:Input:STOP", output);
    //EXPECT_TRUE(stadic::trim(proc.error()).empty());
    UNLINK("input.txt");
}

TEST(ProcessTests, ProcessTwoProcWithError)
{
    std::ofstream out("input.txt");
    out << "This is the first line" << std::endl;
    out << "There's not much more" << std::endl;
    out << "STOP" << std::endl;
    out.close();

    std::vector<std::string> args0;
    args0.push_back("-R");
    stadic::Process proc0(PROGRAM, args0);
    proc0.setStandardInputFile("input.txt");
    proc0.setStandardErrorFile("error.txt");

    std::vector<std::string> args1;
    args1.push_back("-r");
    stadic::Process proc1(PROGRAM, args1);
    proc1.setStandardOutputFile("output.txt");
    proc0.setStandardOutputProcess(&proc1);

    ASSERT_FALSE(proc0.wait());
    ASSERT_FALSE(proc1.wait());

    proc0.start();
    ASSERT_TRUE(proc0.wait());
    proc1.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_TRUE(proc1.wait());

    std::string output = readFileToString("output.txt");
    // std::string output = stadic::trim(proc.output());
    // Zap control chars...
    // output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:This is the first lineInput:Input:There's not much moreInput:Input:STOP", output);
    
    std::string error = readFileToString("error.txt");
    //std::string error = stadic::trim(proc0.error());
    //error.erase(std::remove_if(error.begin(), error.end(), ::iscntrl), error.end());
    EXPECT_EQ("Error:This is the first lineError:There's not much moreError:STOP", error);

    UNLINK("output.txt");
    UNLINK("input.txt");
    UNLINK("error.txt");
}

TEST(ProcessTests, Process5NoStdErr)
{
    std::ofstream out("input.txt");
    out << "STOP" << std::endl;
    out.close();

    std::vector<std::string> args;
    args.push_back("-r");
    stadic::Process proc0(PROGRAM, args);
    proc0.setStandardInputFile("input.txt");

    stadic::Process proc1(PROGRAM, args);
    proc0.setStandardOutputProcess(&proc1);

    stadic::Process proc2(PROGRAM, args);
    proc1.setStandardOutputProcess(&proc2);

    stadic::Process proc3(PROGRAM, args);
    proc2.setStandardOutputProcess(&proc3);

    stadic::Process proc4(PROGRAM, args);
    proc3.setStandardOutputProcess(&proc4);
    proc4.setStandardOutputFile("output.txt");
    
    proc0.start();
    proc1.start();
    proc2.start();
    proc3.start();
    proc4.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_TRUE(proc1.wait());
    ASSERT_TRUE(proc2.wait());
    ASSERT_TRUE(proc3.wait());
    ASSERT_TRUE(proc4.wait());

    std::string output = readFileToString("output.txt");
    //std::string output = stadic::trim(proc4.output());
    // Zap control chars...
    //output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:Input:Input:Input:STOP", output);
    //EXPECT_TRUE(stadic::trim(proc.error()).empty());
    UNLINK("output.txt");
    UNLINK("input.txt");
}

/*
TEST(ProcessTests, Process5StdErrFile)
{
    std::ofstream out("input.txt");
    out << "STOP" << std::endl;
    out.close();

    std::vector<std::string> args;
    args.push_back("-R");
    stadic::Process proc0(PROGRAM, args);
    proc0.setStandardInputFile("input.txt");

    stadic::Process proc1(PROGRAM, args);
    proc0.setStandardOutputProcess(&proc1);

    stadic::Process proc2(PROGRAM, args);
    proc1.setStandardOutputProcess(&proc2);

    stadic::Process proc3(PROGRAM, args);
    proc3.setStandardErrorFile("error.txt");
    proc2.setStandardOutputProcess(&proc3);

    stadic::Process proc4(PROGRAM, args);
    proc3.setStandardOutputProcess(&proc4);

    proc0.start();
    proc1.start();
    proc2.start();
    proc3.start();
    proc4.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_TRUE(proc1.wait());
    ASSERT_TRUE(proc2.wait());
    ASSERT_TRUE(proc3.wait());
    ASSERT_TRUE(proc4.wait());

    //std::string output = stadic::trim(proc4.output());
    // Zap control chars...
    //output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:Input:Input:Input:STOP", stadic::trim(proc4.output()));
    EXPECT_EQ("Error:STOP", stadic::trim(proc0.error()));
    EXPECT_EQ("Error:Input:STOP", stadic::trim(proc1.error()));
    EXPECT_EQ("Error:Input:Input:STOP", stadic::trim(proc2.error()));
    std::ifstream err("error.txt");
    std::string errorString((std::istreambuf_iterator<char>(err)), std::istreambuf_iterator<char>());
    err.close();
    errorString.erase(std::remove_if(errorString.begin(), errorString.end(), ::iscntrl), errorString.end());
    EXPECT_EQ("Error:Input:Input:Input:STOP", errorString);
    EXPECT_EQ("Error:Input:Input:Input:Input:STOP", stadic::trim(proc4.error()));
    EXPECT_TRUE(proc0.output().empty());
    EXPECT_TRUE(proc1.output().empty());
    EXPECT_TRUE(proc2.output().empty());
    EXPECT_TRUE(proc3.output().empty());
    UNLINK("input.txt");
    UNLINK("error.txt");
}

TEST(ProcessTests, Process5StdErrFileStdOutFile)
{
    std::ofstream out("input.txt");
    out << "STOP" << std::endl;
    out.close();

    std::vector<std::string> args;
    args.push_back("-R");
    stadic::Process proc0(PROGRAM, args);
    proc0.setStandardInputFile("input.txt");

    stadic::Process proc1(PROGRAM, args);
    proc0.setStandardOutputProcess(&proc1);

    stadic::Process proc2(PROGRAM, args);
    proc1.setStandardOutputProcess(&proc2);

    stadic::Process proc3(PROGRAM, args);
    proc3.setStandardErrorFile("error.txt");
    proc2.setStandardOutputProcess(&proc3);

    stadic::Process proc4(PROGRAM, args);
    proc4.setStandardOutputFile("output.txt");
    proc3.setStandardOutputProcess(&proc4);

    proc0.start();
    proc1.start();
    proc2.start();
    proc3.start();
    proc4.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_TRUE(proc1.wait());
    ASSERT_TRUE(proc2.wait());
    ASSERT_TRUE(proc3.wait());
    ASSERT_TRUE(proc4.wait());

    EXPECT_EQ("Error:STOP", stadic::trim(proc0.error()));
    EXPECT_EQ("Error:Input:STOP", stadic::trim(proc1.error()));
    EXPECT_EQ("Error:Input:Input:STOP", stadic::trim(proc2.error()));
    std::ifstream err("error.txt");
    std::string errorString((std::istreambuf_iterator<char>(err)), std::istreambuf_iterator<char>());
    err.close();
    errorString.erase(std::remove_if(errorString.begin(), errorString.end(), ::iscntrl), errorString.end());
    EXPECT_EQ("Error:Input:Input:Input:STOP", errorString);
    std::ifstream readout("output.txt");
    std::string outputString((std::istreambuf_iterator<char>(readout)), std::istreambuf_iterator<char>());
    out.close();
    outputString.erase(std::remove_if(outputString.begin(), outputString.end(), ::iscntrl), outputString.end());
    EXPECT_EQ("Input:Input:Input:Input:Input:STOP", outputString);
    EXPECT_EQ("Error:Input:Input:Input:Input:STOP", stadic::trim(proc4.error()));
    EXPECT_TRUE(proc0.output().empty());
    EXPECT_TRUE(proc1.output().empty());
    EXPECT_TRUE(proc2.output().empty());
    EXPECT_TRUE(proc3.output().empty());
    UNLINK("input.txt");
    UNLINK("error.txt");
}

TEST(ProcessTests, Process5StdErrFileStdOutFile)
{
    std::ofstream out("input.txt");
    out << "STOP" << std::endl;
    out.close();

    std::vector<std::string> args;
    args.push_back("-R");
    stadic::Process proc0(PROGRAM, args);
    proc0.setStandardInputFile("input.txt");

    stadic::Process proc1(PROGRAM, args);
    proc0.setStandardOutputProcess(&proc1);

    stadic::Process proc2(PROGRAM, args);
    proc1.setStandardOutputProcess(&proc2);

    stadic::Process proc3(PROGRAM, args);
    proc3.setStandardErrorFile("error.txt");
    proc2.setStandardOutputProcess(&proc3);

    stadic::Process proc4(PROGRAM, args);
    proc4.setStandardOutputFile("output.txt");
    proc3.setStandardOutputProcess(&proc4);

    proc0.start();
    proc1.start();
    proc2.start();
    proc3.start();
    proc4.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_TRUE(proc1.wait());
    ASSERT_TRUE(proc2.wait());
    ASSERT_TRUE(proc3.wait());
    ASSERT_TRUE(proc4.wait());

    EXPECT_EQ("Error:STOP", stadic::trim(proc0.error()));
    EXPECT_EQ("Error:Input:STOP", stadic::trim(proc1.error()));
    EXPECT_EQ("Error:Input:Input:STOP", stadic::trim(proc2.error()));
    std::ifstream err("error.txt");
    std::string errorString((std::istreambuf_iterator<char>(err)), std::istreambuf_iterator<char>());
    err.close();
    errorString.erase(std::remove_if(errorString.begin(), errorString.end(), ::iscntrl), errorString.end());
    EXPECT_EQ("Error:Input:Input:Input:STOP", errorString);
    std::ifstream readout("output.txt");
    std::string outputString((std::istreambuf_iterator<char>(readout)), std::istreambuf_iterator<char>());
    out.close();
    outputString.erase(std::remove_if(outputString.begin(), outputString.end(), ::iscntrl), outputString.end());
    EXPECT_EQ("Input:Input:Input:Input:Input:STOP", outputString);
    EXPECT_EQ("Error:Input:Input:Input:Input:STOP", stadic::trim(proc4.error()));
    EXPECT_TRUE(proc0.output().empty());
    EXPECT_TRUE(proc1.output().empty());
    EXPECT_TRUE(proc2.output().empty());
    EXPECT_TRUE(proc3.output().empty());
    UNLINK("input.txt");
    UNLINK("error.txt");
}
*/

TEST(ProcessTests, Process5StdErrFileStdOutFile)
{
    std::ofstream out("input.txt");
    out << "STOP" << std::endl;
    out.close();

    std::vector<std::string> args;
    args.push_back("-R");
    stadic::Process proc0(PROGRAM, args);
    proc0.setStandardInputFile("input.txt");
    proc0.setStandardErrorFile("error0.txt");

    stadic::Process proc1(PROGRAM, args);
    proc0.setStandardOutputProcess(&proc1);
    proc1.setStandardErrorFile("error1.txt");

    stadic::Process proc2(PROGRAM, args);
    proc1.setStandardOutputProcess(&proc2);
    proc2.setStandardErrorFile("error2.txt");

    stadic::Process proc3(PROGRAM, args);
    proc2.setStandardOutputProcess(&proc3);
    proc3.setStandardErrorFile("error3.txt");

    stadic::Process proc4(PROGRAM, args);
    proc4.setStandardOutputFile("output.txt");
    proc3.setStandardOutputProcess(&proc4);
    proc4.setStandardErrorFile("error4.txt");

    proc0.start();
    proc1.start();
    proc2.start();
    proc3.start();
    proc4.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_TRUE(proc1.wait());
    ASSERT_TRUE(proc2.wait());
    ASSERT_TRUE(proc3.wait());
    ASSERT_TRUE(proc4.wait());

    std::string error0 = readFileToString("error0.txt");
    std::string error1 = readFileToString("error1.txt");
    std::string error2 = readFileToString("error2.txt");
    std::string error3 = readFileToString("error3.txt");
    std::string error4 = readFileToString("error4.txt");

    EXPECT_EQ("Error:STOP", error0);
    EXPECT_EQ("Error:Input:STOP", error1);
    EXPECT_EQ("Error:Input:Input:STOP", error2);
    EXPECT_EQ("Error:Input:Input:Input:STOP", error3);
    EXPECT_EQ("Error:Input:Input:Input:Input:STOP", error4);
   
    std::string output = readFileToString("output.txt");
    EXPECT_EQ("Input:Input:Input:Input:Input:STOP", output);
    
    UNLINK("input.txt");
    UNLINK("error0.txt");
    UNLINK("error1.txt");
    UNLINK("error2.txt");
    UNLINK("error3.txt");
    UNLINK("error4.txt");
    UNLINK("output.txt");
}
