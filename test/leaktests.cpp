#include "leakcheck.h"
#include "gtest/gtest.h"
#include <QStringList>

TEST(LeakTests, EnclosedModel)
{
    //This model should be fully enclosed
  stadic::LeakCheck leakChecker;
  QStringList radFiles;
  radFiles.clear();
  radFiles.append(":/resources/Simple.rad");
  radFiles.append(":/resources/material.rad");
  ASSERT_TRUE(leakChecker.setRadFile(radFiles));
  QStringList layerNames;
  layerNames.clear();
  layerNames.append("l_floor");
  ASSERT_TRUE(leakChecker.setFloorLayers(layerNames));
  ASSERT_TRUE(leakChecker.setX(120));
  ASSERT_TRUE(leakChecker.setY(120));
  ASSERT_TRUE(leakChecker.setZ(30));
  ASSERT_TRUE(leakChecker.setReflectance(1));
  ASSERT_TRUE(leakChecker.setWorkingDirectory(":/tmp/"));
  ASSERT_TRUE(leakChecker.isEnclosed());
}

TEST(LeakTests, LeakModel)
{

}
