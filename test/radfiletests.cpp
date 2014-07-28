#include "radfiledata.h"
#include "gtest/gtest.h"
#include <iostream>

TEST(RadFileTests, ParseRadFile)
{
  stadic::RadFileData radData;
  ASSERT_TRUE(radData.addRad(":/resources/Simple.rad"));
  ASSERT_TRUE(radData.addRad(":/resources/material.rad"));
  //EXPECT_EQ("prj1",controlFile.projectName());
  //Testing First Polygon for contents
  EXPECT_EQ("l_floor", radData.geometry().at(0)->modifier());
  EXPECT_EQ(stadic::RadPrimitive::Polygon, radData.geometry().at(0)->type());
  EXPECT_EQ("l_floor.0.1", radData.geometry().at(0)->name());
  EXPECT_EQ(0, radData.geometry().at(0)->arg1().size());
  EXPECT_EQ(0, radData.geometry().at(0)->arg2().size());
  EXPECT_EQ(12, radData.geometry().at(0)->arg3().size());
  EXPECT_EQ(0, radData.geometry().at(0)->arg3().at(0).toDouble());
  EXPECT_EQ(240, radData.geometry().at(0)->arg3().at(3).toDouble());
  EXPECT_EQ(240, radData.geometry().at(0)->arg3().at(6).toDouble());
  EXPECT_EQ(240, radData.geometry().at(0)->arg3().at(10).toDouble());

  //Testing Last Polygon for contents
  EXPECT_EQ("l_ceiling", radData.geometry().at(radData.geometry().size()-1)->modifier());
  EXPECT_EQ(stadic::RadPrimitive::Polygon, radData.geometry().at(radData.geometry().size()-1)->type());
  EXPECT_EQ("l_ceiling.39.1", radData.geometry().at(radData.geometry().size()-1)->name());
  EXPECT_EQ(0, radData.geometry().at(radData.geometry().size()-1)->arg1().size());
  EXPECT_EQ(0, radData.geometry().at(radData.geometry().size()-1)->arg2().size());
  ASSERT_TRUE(12== radData.geometry().at(radData.geometry().size()-1)->arg3().size());
  EXPECT_EQ(0, radData.geometry().at(radData.geometry().size()-1)->arg3().at(0).toDouble());
  EXPECT_EQ(0, radData.geometry().at(radData.geometry().size()-1)->arg3().at(3).toDouble());
  EXPECT_EQ(240, radData.geometry().at(radData.geometry().size()-1)->arg3().at(6).toDouble());
  EXPECT_EQ(0, radData.geometry().at(radData.geometry().size()-1)->arg3().at(10).toDouble());

  //Testing to ensure all of the polygons are read in
  EXPECT_EQ(36, radData.geometry().size());

  //Testing first material for contents
  EXPECT_EQ("void", radData.materials().at(0)->modifier());
  EXPECT_EQ(stadic::RadPrimitive::Plastic, radData.materials().at(0)->type());
  EXPECT_EQ("l_wall", radData.materials().at(0)->name());
  EXPECT_EQ(0, radData.materials().at(0)->arg1().size());
  EXPECT_EQ(0, radData.materials().at(0)->arg2().size());
  ASSERT_TRUE(5== radData.materials().at(0)->arg3().size());
  EXPECT_EQ(.5, radData.materials().at(0)->arg3().at(0).toDouble());
  EXPECT_EQ(.5, radData.materials().at(0)->arg3().at(1).toDouble());
  EXPECT_EQ(.5, radData.materials().at(0)->arg3().at(2).toDouble());
  EXPECT_EQ(0, radData.materials().at(0)->arg3().at(3).toDouble());
  EXPECT_EQ(0, radData.materials().at(0)->arg3().at(4).toDouble());

  //Testing second material for contents
  EXPECT_EQ("void", radData.materials().at(1)->modifier());
  EXPECT_EQ(stadic::RadPrimitive::Glass, radData.materials().at(1)->type());
  EXPECT_EQ("l_window", radData.materials().at(1)->name());
  EXPECT_EQ(0, radData.materials().at(1)->arg1().size());
  EXPECT_EQ(0, radData.materials().at(1)->arg2().size());
  ASSERT_TRUE(3== radData.materials().at(1)->arg3().size());
  EXPECT_EQ(.65, radData.materials().at(1)->arg3().at(0).toDouble());
  EXPECT_EQ(.65, radData.materials().at(1)->arg3().at(1).toDouble());
  EXPECT_EQ(.65, radData.materials().at(1)->arg3().at(2).toDouble());

  //Testing last material for contents
  EXPECT_EQ("void", radData.materials().at(radData.materials().size()-1)->modifier());
  EXPECT_EQ(stadic::RadPrimitive::Plastic, radData.materials().at(radData.materials().size()-1)->type());
  EXPECT_EQ("l_ceiling", radData.materials().at(radData.materials().size()-1)->name());
  EXPECT_EQ(0, radData.materials().at(radData.materials().size()-1)->arg1().size());
  EXPECT_EQ(0, radData.materials().at(radData.materials().size()-1)->arg2().size());
  ASSERT_TRUE(5== radData.materials().at(radData.materials().size()-1)->arg3().size());
  EXPECT_EQ(.8, radData.materials().at(radData.materials().size()-1)->arg3().at(0).toDouble());
  EXPECT_EQ(.8, radData.materials().at(radData.materials().size()-1)->arg3().at(1).toDouble());
  EXPECT_EQ(.8, radData.materials().at(radData.materials().size()-1)->arg3().at(2).toDouble());
  EXPECT_EQ(0, radData.materials().at(radData.materials().size()-1)->arg3().at(3).toDouble());
  EXPECT_EQ(0, radData.materials().at(radData.materials().size()-1)->arg3().at(4).toDouble());

  //Testing to ensure all of the materials are read in
  EXPECT_EQ(6, radData.materials().size());
}
