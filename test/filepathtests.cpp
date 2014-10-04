#include "objects.h"
#include "functions.h"
#include "gtest/gtest.h"
#include <string>
#include <fstream>
#ifdef _MSC_VER
#define UNLINK _unlink
#else
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
    //There may need to be a delay inserted here so the updated time actually changes
    std::ofstream reWrite(testString);
    reWrite << "I'm doing this as hard as I can";
    reWrite.close();

    EXPECT_TRUE(file.exists());
    EXPECT_TRUE(file.isFile());
    EXPECT_FALSE(file.isDir());
    EXPECT_TRUE(file.isUpdated());

}