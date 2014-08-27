#include "leakcheck.h"
#include "gtest/gtest.h"
#include <QStringList>
#include <QDir>
#include <QFile>
#include <iostream>

TEST(LeakTests, EnclosedModel)
{
  //This model should be fully enclosed
  stadic::LeakCheck leakChecker;
  QStringList radFiles;
  radFiles.clear();
  QDir wDir;
  wDir=QDir::currentPath();
  wDir.cdUp();
  wDir.cdUp();
  if (!wDir.cd("test/tmp/")){
    std::cout<<"The program failed to move to the temporary directory."<<std::endl;
    EXIT_FAILURE;
  }
  if (!QFile::copy(":/resources/Simple.rad",wDir.path()+"/Simple.rad")){
    std::cout<<"The program failed to copy the geometry resource file."<<std::endl;
    EXIT_FAILURE;
  }
  if (!QFile::copy(":/resources/material.rad",wDir.path()+"/material.rad")){
    std::cout<<"The program failed to copy the material resource file."<<std::endl;
    EXIT_FAILURE;
  }
  radFiles.append(wDir.path()+"/Simple.rad");
  radFiles.append(wDir.path()+"/material.rad");
  //radFiles.append(":/resources/Simple.rad");
  //radFiles.append(":/resources/material.rad");
  //radFiles.append("c:/CPrograms/STADIC/test/resources/Simple.rad");
  //radFiles.append("c:/CPrograms/STADIC/test/resources/material.rad");
  ASSERT_TRUE(leakChecker.setRadFile(radFiles));
  QStringList layerNames;
  layerNames.clear();
  layerNames.append("l_floor");
  ASSERT_TRUE(leakChecker.setFloorLayers(layerNames));
  ASSERT_TRUE(leakChecker.setX(120));
  ASSERT_TRUE(leakChecker.setY(120));
  ASSERT_TRUE(leakChecker.setZ(30));
  ASSERT_TRUE(leakChecker.setReflectance(1));
  ASSERT_TRUE(leakChecker.setWorkingDirectory("c:/CPrograms/STADIC/test/tmp/"));
  ASSERT_TRUE(leakChecker.isEnclosed());
}

TEST(LeakTests, LeakModel)
{
  //This model should contain a 10" x 10" square hole along the east side of the ceiling
  stadic::LeakCheck leakChecker;
  QStringList radFiles;
  radFiles.clear();
  //radFiles.append(":/resources/SimpleHole.rad");
  //radFiles.append(":/resources/material.rad");
  radFiles.append("c:/CPrograms/STADIC/test/resources/SimpleHole.rad");
  radFiles.append("c:/CPrograms/STADIC/test/resources/material.rad");
  ASSERT_TRUE(leakChecker.setRadFile(radFiles));
  QStringList layerNames;
  layerNames.clear();
  layerNames.append("l_floor");
  ASSERT_TRUE(leakChecker.setFloorLayers(layerNames));
  ASSERT_TRUE(leakChecker.setX(120));
  ASSERT_TRUE(leakChecker.setY(120));
  ASSERT_TRUE(leakChecker.setZ(30));
  ASSERT_TRUE(leakChecker.setReflectance(1));
  ASSERT_TRUE(leakChecker.setWorkingDirectory("c:/CPrograms/STADIC/test/tmp/"));
  ASSERT_FALSE(leakChecker.isEnclosed());

}
