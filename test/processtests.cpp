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
 *    and the following Disclaimer.
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

#include "stadicprocess.h"
#include "functions.h"
#include "gtest/gtest.h"
#include <string>
#include <fstream>
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
    EXPECT_TRUE(proc.output().size() > 0);
}

TEST(ProcessTests, ProcessProgramArgs)
{
    std::vector<std::string> days = { "Sun", "Mon", "Tue", "Wed", "Thu", "Fri", "Sat" };
#ifdef _WIN32
    SYSTEMTIME time;
    GetLocalTime(&time);
    std::string datestring = days[time.wDayOfWeek] + " " + stadic::toString(time.wDay) + "/" 
        + stadic::toString(time.wMonth) + "/" + stadic::toString(time.wYear);
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
    std::string datestring = "\"" + days[current->tm_wday] + " " + stadic::toString(current->tm_year+1900) + "-"
            + stadic::toString(current->tm_mon+1) + "-" + stadic::toString(current->tm_mday) + "\"";
#endif
    proc.start();
    EXPECT_TRUE(proc.wait());
    EXPECT_EQ(datestring, stadic::trim(proc.output()));
}


//#ifdef _WIN32
#define PROGRAM "testprogram"
//#else
//#define PROGRAM "./testprogram"
//#endif
TEST(ProcessTests, ProcessCaptureOutErr)
{
    stadic::Process proc(PROGRAM);
    proc.start();
    ASSERT_TRUE(proc.wait());
    EXPECT_EQ("This is the standard output", stadic::trim(proc.output()));
    EXPECT_EQ("This is the standard error", stadic::trim(proc.error()));
}

TEST(ProcessTests, ProcessOutErrFiles)
{
    std::stringstream stream;
    stadic::Process proc(PROGRAM);
    proc.setStandardErrorFile("error0.txt");
    proc.setStandardOutputFile("output0.txt");
    proc.start();
    ASSERT_TRUE(proc.wait());
    std::ifstream err("error0.txt");
    std::string errorString((std::istreambuf_iterator<char>(err)), std::istreambuf_iterator<char>());
    err.close();
    UNLINK("error0.txt");
    std::ifstream out("output0.txt");
    std::string outputString((std::istreambuf_iterator<char>(out)), std::istreambuf_iterator<char>());
    out.close();
    UNLINK("output0.txt");
    EXPECT_EQ("This is the standard output", stadic::trim(outputString));
    EXPECT_EQ("This is the standard error", stadic::trim(errorString));
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
    std::ifstream err("error.txt");
    std::string errorString((std::istreambuf_iterator<char>(err)), std::istreambuf_iterator<char>());
    err.close();
    UNLINK("error.txt");
    std::ifstream out("output.txt");
    std::string outputString((std::istreambuf_iterator<char>(out)), std::istreambuf_iterator<char>());
    out.close();
    UNLINK("output.txt");
    EXPECT_EQ("Here are some standard output letters: xxxxxxxxx", stadic::trim(outputString));
    EXPECT_EQ("Here are some standard error letters: xxxxxxxxx", stadic::trim(errorString));
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
    proc.start();
    ASSERT_TRUE(proc.wait());
    std::string output = stadic::trim(proc.output());
    // Zap control chars...
    output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:This is line 1Input:This is line 2Input:STOP", output);
    EXPECT_TRUE(stadic::trim(proc.error()).empty());
    UNLINK("input.txt");
}

TEST(ProcessTests, ProcessMultiples1)
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

    proc0.setStandardOutputProcess(&proc1);

    ASSERT_FALSE(proc0.wait());
    ASSERT_FALSE(proc1.wait());

    proc0.start();
    ASSERT_FALSE(proc0.wait());
    proc1.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_FALSE(proc1.wait());

    std::string output = stadic::trim(proc1.output());
    // Zap control chars...
    output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:This is the first lineInput:Input:There's not much moreInput:Input:STOP", output);
    //EXPECT_TRUE(stadic::trim(proc.error()).empty());
    UNLINK("input.txt");
}

TEST(ProcessTests, ProcessMultiples2)
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

    proc0.setStandardOutputProcess(&proc1);

    ASSERT_FALSE(proc0.wait());
    ASSERT_FALSE(proc1.wait());

    proc1.start();
    ASSERT_FALSE(proc0.wait());
    ASSERT_FALSE(proc1.wait());
    proc0.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_FALSE(proc1.wait());

    std::string output = stadic::trim(proc1.output());
    // Zap control chars...
    output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:This is the first lineInput:Input:There's not much moreInput:Input:STOP", output);
    //EXPECT_TRUE(stadic::trim(proc.error()).empty());
    UNLINK("input.txt");
}

TEST(ProcessTests, ProcessMultiples3)
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

    std::vector<std::string> args1;
    args1.push_back("-R");
    stadic::Process proc1(PROGRAM, args1);

    proc0.setStandardOutputProcess(&proc1);

    ASSERT_FALSE(proc0.wait());
    ASSERT_FALSE(proc1.wait());

    proc1.start();
    ASSERT_FALSE(proc0.wait());
    ASSERT_FALSE(proc1.wait());
    proc0.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_FALSE(proc1.wait());

    std::string output = stadic::trim(proc1.output());
    // Zap control chars...
    output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:This is the first lineInput:Input:There's not much moreInput:Input:STOP", output);
    //EXPECT_TRUE(stadic::trim(proc.error()).empty());
    UNLINK("input.txt");

    std::string error = stadic::trim(proc0.error());
    error.erase(std::remove_if(error.begin(), error.end(), ::iscntrl), error.end());
    EXPECT_EQ("Error:This is the first lineError:There's not much moreError:STOP",error);

    error = stadic::trim(proc1.error());
    error.erase(std::remove_if(error.begin(), error.end(), ::iscntrl), error.end());
    EXPECT_EQ("Error:Input:This is the first lineError:Input:There's not much moreError:Input:STOP",error);
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

    
    proc0.start();
    proc1.start();
    proc2.start();
    proc3.start();
    proc4.start();

    ASSERT_TRUE(proc0.wait());
    ASSERT_FALSE(proc1.wait());
    ASSERT_FALSE(proc2.wait());
    ASSERT_FALSE(proc3.wait());
    ASSERT_FALSE(proc4.wait());

    std::string output = stadic::trim(proc4.output());
    // Zap control chars...
    output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:Input:Input:Input:STOP", output);
    //EXPECT_TRUE(stadic::trim(proc.error()).empty());
    UNLINK("input.txt");
}

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
    ASSERT_FALSE(proc1.wait());
    ASSERT_FALSE(proc2.wait());
    ASSERT_FALSE(proc3.wait());
    ASSERT_FALSE(proc4.wait());

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
    ASSERT_FALSE(proc1.wait());
    ASSERT_FALSE(proc2.wait());
    ASSERT_FALSE(proc3.wait());
    ASSERT_FALSE(proc4.wait());

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
