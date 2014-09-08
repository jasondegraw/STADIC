
#include "objects.h"
#include "gtest/gtest.h"
#include <string>

TEST(ObjectsTests, Directory)
{
  std::string testString = "c:/CPrograms/STADIC/";
  stadic::Dir dir;
  dir.setPath(testString);
  EXPECT_TRUE(dir.exists());
  EXPECT_TRUE(dir.isDir());
  testString = "c:/DOESNOTEXIST/";
  dir.setPath(testString);
  EXPECT_FALSE(dir.exists());
  testString="c:/CPrograms/STADIC";
  EXPECT_FALSE(dir.setPath(testString));
}

TEST(ObjectsTests, File)
{
    std::string testString = "c:/CPrograms/STADIC/README.md";
    stadic::File file;
    file.setPath(testString);
    EXPECT_TRUE(file.exists());
    EXPECT_TRUE(file.isFile());
    EXPECT_TRUE(file.lastMod());
    EXPECT_FALSE(file.isUpdated());
    EXPECT_EQ("README.md",file.name());
    EXPECT_EQ("c:/CPrograms/STADIC/",file.dir().toString());
    testString = "c:/CPrograms/STADIC/DOESNOTEXIST.txt";
    file.setPath(testString);
    EXPECT_FALSE(file.exists());
}
