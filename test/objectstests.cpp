#include "objects.h"
#include "gtest/gtest.h"
#include <string>

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

#ifdef _WIN32
  testString = "C:\\Windows\\";
#else
  testString = "/usr/";
#endif
  stadic::FilePath dir1(testString);
  EXPECT_TRUE(dir1.exists());
  EXPECT_TRUE(dir1.isDir());
  EXPECT_FALSE(dir1.isFile());

  stadic::FilePath dir2("DOESNOTEXIST");
  EXPECT_FALSE(dir2.exists());
}

TEST(ObjectsTests, FilePathFile)
{
  /*
    std::string testString = "c:/CPrograms/STADIC/README.md";
    stadic::FilePath file(testString);
    EXPECT_TRUE(file.exists());
    EXPECT_TRUE(file.isFile());
    EXPECT_FALSE(file.isUpdated());
    testString = "c:/CPrograms/STADIC/DOESNOTEXIST.txt";
    stadic::FilePath file2(testString);
    EXPECT_FALSE(file2.exists());
    */
}
