
#include "objects.h"
#include "gtest/gtest.h"
#include <string>

TEST(ObjectsTests, Directory)
{
  std::string testString = "c:/CPrograms/STADIC/";
  stadic::FileSystem dir;
  dir.setPath(testString);
  EXPECT_TRUE(dir.exists());
  EXPECT_TRUE(dir.isDir());
  EXPECT_FALSE(dir.isFile());
  testString = "c:/DOESNOTEXIST/";
  dir.setPath(testString);
  EXPECT_FALSE(dir.exists());
}

TEST(ObjectsTests, File)
{
    std::string testString = "c:/CPrograms/STADIC/README.md";
    stadic::FileSystem file;
    file.setPath(testString);
    EXPECT_TRUE(file.exists());
    EXPECT_TRUE(file.isFile());
    EXPECT_FALSE(file.isDir());
    EXPECT_TRUE(file.lastMod());
    EXPECT_FALSE(file.isUpdated());
    testString = "c:/CPrograms/STADIC/DOESNOTEXIST.txt";
    file.setPath(testString);
    EXPECT_FALSE(file.exists());
}
