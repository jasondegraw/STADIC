#include "objects.h"
#include "functions.h"
#include "gtest/gtest.h"
#include <string>
#include <fstream>
//#include <sstream>
//#include <iostream>

TEST(ObjectsTests, FilePathDirectory)
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

TEST(ObjectsTests, FilePathFile)
{
    std::string testString = "testfile.txt";
    unlink(testString.c_str());

    stadic::FilePath file(testString);
    EXPECT_FALSE(file.exists());

    std::ofstream testOut(testString);
    testOut << "This is a test file";
    testOut.close();

    EXPECT_TRUE(file.exists());
    EXPECT_TRUE(file.isFile());
    EXPECT_FALSE(file.isDir());
    EXPECT_FALSE(file.isUpdated());

    std::ofstream reWrite(testString);
    reWrite << "I'm doing this as hard as I can";
    reWrite.close();

    EXPECT_TRUE(file.exists());
    EXPECT_TRUE(file.isFile());
    EXPECT_FALSE(file.isDir());
    EXPECT_TRUE(file.isUpdated());

}

//#ifdef _WIN32
#define PROGRAM "testprogram"
//#else
//#define PROGRAM "./testprogram"
//#endif
TEST(ObjectsTests, ProcessCaptureOutErr)
{
    stadic::Process proc(PROGRAM);
    proc.start();
    proc.wait();
    EXPECT_EQ("This is the standard output", stadic::trim(proc.output()));
    EXPECT_EQ("This is the standard error", stadic::trim(proc.error()));
}

TEST(ObjectsTests, ProcessOutErrFiles)
{
    std::stringstream stream;
    stadic::Process proc(PROGRAM);
    proc.setStandardErrorFile("error.txt");
    proc.setStandardOutputFile("output.txt");
    proc.start();
    proc.wait();
    std::ifstream err("error.txt");
    std::string errorString((std::istreambuf_iterator<char>(err)), std::istreambuf_iterator<char>());
    err.close();
    unlink("error.txt");
    std::ifstream out("output.txt");
    std::string outputString((std::istreambuf_iterator<char>(out)), std::istreambuf_iterator<char>());
    out.close();
    unlink("output.txt");
    EXPECT_EQ("This is the standard output", stadic::trim(outputString));
    EXPECT_EQ("This is the standard error", stadic::trim(errorString));
}

TEST(ObjectsTests, ProcessArgsOutErrFiles)
{
    std::stringstream stream;
    std::vector<std::string> args;
    args.push_back("-x");
    stadic::Process proc(PROGRAM,args);
    proc.setStandardErrorFile("error.txt");
    proc.setStandardOutputFile("output.txt");
    proc.start();
    proc.wait();
    std::ifstream err("error.txt");
    std::string errorString((std::istreambuf_iterator<char>(err)), std::istreambuf_iterator<char>());
    err.close();
    unlink("error.txt");
    std::ifstream out("output.txt");
    std::string outputString((std::istreambuf_iterator<char>(out)), std::istreambuf_iterator<char>());
    out.close();
    unlink("output.txt");
    EXPECT_EQ("Here are some standard output letters: xxxxxxxxx", stadic::trim(outputString));
    EXPECT_EQ("Here are some standard error letters: xxxxxxxxx", stadic::trim(errorString));
}

TEST(ObjectsTests, ProcessInputFile)
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
    proc.wait();
    std::string output = stadic::trim(proc.output());
    // Zap control chars...
    output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:This is line 1Input:This is line 2Input:STOP", output);
    EXPECT_TRUE(stadic::trim(proc.error()).empty());
    unlink("input.txt");
}

TEST(ObjectsTests, ProcessMultiples1)
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

    proc0.start();
    proc1.start();

    proc0.wait();
    proc1.wait();

    std::string output = stadic::trim(proc1.output());
    // Zap control chars...
    output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:This is the first lineInput:Input:There's not much moreInput:Input:STOP", output);
    //EXPECT_TRUE(stadic::trim(proc.error()).empty());
    unlink("input.txt");
}

TEST(ObjectsTests, ProcessMultiples2)
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

    proc1.start();
    proc0.start();

    proc0.wait();
    proc1.wait();

    std::string output = stadic::trim(proc1.output());
    // Zap control chars...
    output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:This is the first lineInput:Input:There's not much moreInput:Input:STOP", output);
    //EXPECT_TRUE(stadic::trim(proc.error()).empty());
    unlink("input.txt");
}

TEST(ObjectsTests, ProcessMultiplesMore1)
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

    proc0.wait();
    proc1.wait();
    proc2.wait();
    proc3.wait();
    proc4.wait();

    std::string output = stadic::trim(proc4.output());
    // Zap control chars...
    output.erase(std::remove_if(output.begin(), output.end(), ::iscntrl), output.end());
    EXPECT_EQ("Input:Input:Input:Input:Input:STOP", output);
    //EXPECT_TRUE(stadic::trim(proc.error()).empty());
    unlink("input.txt");
}

TEST(ObjectsTests, ProcessMultiplesMore2)
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

    proc0.wait();
    proc1.wait();
    proc2.wait();
    proc3.wait();
    proc4.wait();

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
    unlink("input.txt");
    unlink("error.txt");
}