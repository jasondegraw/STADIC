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
  std::vector<std::string> radFiles = { "Simple.rad", "material.rad" };
  /*
  radFiles.clear();

  QDir wDir;
  wDir=QDir::currentPath();
  wDir.cdUp();
  wDir.cdUp();
  if (!wDir.cd("test/tmp/")){
    std::cout<<"The program failed to move to the temporary directory."<<std::endl;
    EXIT_FAILURE;
  }
  QFile::copy(":/resources/Simple.rad",wDir.path()+"/Simple.rad");
  QFile::copy(":/resources/material.rad",wDir.path()+"/material.rad");
  radFiles.push_back(wDir.path().toStdString()+"/Simple.rad");
  radFiles.push_back(wDir.path().toStdString()+"/material.rad");*/
  //radFiles.append("Simple.rad");
  //radFiles.append("material.rad");
  //radFiles.append("c:/CPrograms/STADIC/test/resources/Simple.rad");
  //radFiles.append("c:/CPrograms/STADIC/test/resources/material.rad");
  ASSERT_TRUE(leakChecker.setRadFile(radFiles));
  std::vector<std::string> layerNames = { "l_floor" };
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
  std::vector<std::string> radFiles;
  radFiles.clear();
  QDir wDir;
  wDir=QDir::currentPath();
  wDir.cdUp();
  wDir.cdUp();
  if (!wDir.cd("test/tmp/")){
    std::cout<<"The program failed to move to the temporary directory."<<std::endl;
    EXIT_FAILURE;
  }
  QFile::copy(":/resources/SimpleHole.rad",wDir.path()+"/SimpleHole.rad");
  QFile::copy(":/resources/material.rad",wDir.path()+"/material.rad");
  radFiles.push_back(wDir.path().toStdString()+"/SimpleHole.rad");
  radFiles.push_back(wDir.path().toStdString()+"/material.rad");
  //radFiles.append(":/resources/SimpleHole.rad");
  //radFiles.append(":/resources/material.rad");
  //radFiles.append("c:/CPrograms/STADIC/test/resources/SimpleHole.rad");
  //radFiles.append("c:/CPrograms/STADIC/test/resources/material.rad");
  ASSERT_TRUE(leakChecker.setRadFile(radFiles));
  std::vector<std::string> layerNames;
  layerNames.clear();
  layerNames.push_back("l_floor");
  ASSERT_TRUE(leakChecker.setFloorLayers(layerNames));
  ASSERT_TRUE(leakChecker.setX(120));
  ASSERT_TRUE(leakChecker.setY(120));
  ASSERT_TRUE(leakChecker.setZ(30));
  ASSERT_TRUE(leakChecker.setReflectance(1));
  ASSERT_TRUE(leakChecker.setWorkingDirectory("c:/CPrograms/STADIC/test/tmp/"));
  ASSERT_FALSE(leakChecker.isEnclosed());

}
