
#include "objects.h"
#include "gtest/gtest.h"
#include <string>

TEST(ObjectsTests, FilePathDirectory)
{
  std::string testString = "c:/CPrograms/STADIC/";
  stadic::FilePath dir(testString);
  EXPECT_TRUE(dir.exists());
  EXPECT_TRUE(dir.isDir());
  testString = "c:/DOESNOTEXIST/";
  stadic::FilePath dir2(testString);
  EXPECT_FALSE(dir2.exists());
}

TEST(ObjectsTests, FilePathFile)
{
    std::string testString = "c:/CPrograms/STADIC/README.md";
    stadic::FilePath file(testString);
    EXPECT_TRUE(file.exists());
    EXPECT_TRUE(file.isFile());
    EXPECT_FALSE(file.isUpdated());
    testString = "c:/CPrograms/STADIC/DOESNOTEXIST.txt";
    stadic::FilePath file2(testString);
    EXPECT_FALSE(file2.exists());
}
