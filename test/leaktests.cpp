#include "leakcheck.h"
#include "gtest/gtest.h"
#include <iostream>
#include <vector>

TEST(LeakTests, EnclosedModel)
{
  //This model should be fully enclosed
  stadic::LeakCheck leakChecker;
  std::vector<std::string> radFiles = { "Simple.rad", "material.rad" };
  ASSERT_TRUE(leakChecker.setRadFile(radFiles));
  std::vector<std::string> layerNames = { "l_floor" };
  leakChecker.setFloorLayers(layerNames);
  ASSERT_TRUE(leakChecker.setUnits(0));
  ASSERT_TRUE(leakChecker.setReflectance(1));
  ASSERT_TRUE(leakChecker.isEnclosed());

}

TEST(LeakTests, LeakModel)
{
  //This model should contain a 10" x 10" square hole along the east side of the ceiling
  stadic::LeakCheck leakChecker;
  std::vector<std::string> radFiles={"SimpleHole.rad","material.rad"};
  ASSERT_TRUE(leakChecker.setRadFile(radFiles));
  std::vector<std::string> layerNames;
  layerNames.clear();
  layerNames.push_back("l_floor");
  leakChecker.setFloorLayers(layerNames);
  ASSERT_TRUE(leakChecker.setUnits(0));
  ASSERT_TRUE(leakChecker.setReflectance(1));
  ASSERT_FALSE(leakChecker.isEnclosed());

}
