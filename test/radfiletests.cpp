#include "radfiledata.h"
#include "gtest/gtest.h"
#include <iostream>
#include "materialprimitives.h"
#include "functions.h"

TEST(RadFileTests, ParseRadFile)
{
  stadic::RadFileData radData;
  ASSERT_TRUE(radData.addRad("Simple.rad"));
  ASSERT_EQ(36, radData.primitives().size());
  ASSERT_EQ(36, radData.geometry().size());
  ASSERT_EQ(0, radData.materials().size());
  ASSERT_TRUE(radData.addRad("material.rad"));
  //Testing to ensure all of the primitives are read in
  ASSERT_EQ(42, radData.primitives().size());
  ASSERT_EQ(36, radData.geometry().size());
  ASSERT_EQ(6, radData.materials().size());

  //Testing First Polygon for contents
  EXPECT_EQ("l_floor", radData.geometry().at(0)->modifier());
  EXPECT_EQ(stadic::RadPrimitive::Polygon, radData.geometry().at(0)->type());
  EXPECT_EQ("l_floor.0.1", radData.geometry().at(0)->name());
  EXPECT_EQ(0, radData.geometry().at(0)->arg1().size());
  EXPECT_EQ(0, radData.geometry().at(0)->arg2().size());
  EXPECT_EQ(12, radData.geometry().at(0)->arg3().size());
  EXPECT_EQ(0, stadic::toDouble(radData.geometry().at(0)->arg3().at(0)));
  EXPECT_EQ(240, stadic::toDouble(radData.geometry().at(0)->arg3().at(3)));
  EXPECT_EQ(240, stadic::toDouble(radData.geometry().at(0)->arg3().at(6)));
  EXPECT_EQ(240, stadic::toDouble(radData.geometry().at(0)->arg3().at(10)));

  //Testing Last Polygon for contents
  EXPECT_EQ("l_ceiling", radData.geometry().at(radData.geometry().size()-1)->modifier());
  EXPECT_EQ(stadic::RadPrimitive::Polygon, radData.geometry().at(radData.geometry().size()-1)->type());
  EXPECT_EQ("l_ceiling.39.1", radData.geometry().at(radData.geometry().size()-1)->name());
  EXPECT_EQ(0, radData.geometry().at(radData.geometry().size()-1)->arg1().size());
  EXPECT_EQ(0, radData.geometry().at(radData.geometry().size()-1)->arg2().size());
  ASSERT_EQ(12, radData.geometry().at(radData.geometry().size()-1)->arg3().size());
  EXPECT_EQ(0, stadic::toDouble(radData.geometry().at(radData.geometry().size()-1)->arg3().at(0)));
  EXPECT_EQ(0, stadic::toDouble(radData.geometry().at(radData.geometry().size()-1)->arg3().at(3)));
  EXPECT_EQ(240, stadic::toDouble(radData.geometry().at(radData.geometry().size()-1)->arg3().at(6)));
  EXPECT_EQ(0, stadic::toDouble(radData.geometry().at(radData.geometry().size()-1)->arg3().at(10)));

  //Testing first material for contents
  EXPECT_EQ("void", radData.materials().at(0)->modifier());
  EXPECT_EQ(stadic::RadPrimitive::Plastic, radData.materials().at(0)->type());
  EXPECT_EQ("l_wall", radData.materials().at(0)->name());
  EXPECT_EQ(0, radData.materials().at(0)->arg1().size());
  EXPECT_EQ(0, radData.materials().at(0)->arg2().size());
  ASSERT_EQ(5, radData.materials().at(0)->arg3().size());
  EXPECT_EQ(.5, stadic::toDouble(radData.materials().at(0)->arg3().at(0)));
  EXPECT_EQ(.5, stadic::toDouble(radData.materials().at(0)->arg3().at(1)));
  EXPECT_EQ(.5, stadic::toDouble(radData.materials().at(0)->arg3().at(2)));
  EXPECT_EQ(0, stadic::toDouble(radData.materials().at(0)->arg3().at(3)));
  EXPECT_EQ(0, stadic::toDouble(radData.materials().at(0)->arg3().at(4)));

  //Testing second material for contents
  EXPECT_EQ("void", radData.materials().at(1)->modifier());
  EXPECT_EQ(stadic::RadPrimitive::Glass, radData.materials().at(1)->type());
  EXPECT_EQ("l_window", radData.materials().at(1)->name());
  EXPECT_EQ(0, radData.materials().at(1)->arg1().size());
  EXPECT_EQ(0, radData.materials().at(1)->arg2().size());
  ASSERT_EQ(3, radData.materials().at(1)->arg3().size());
  EXPECT_EQ(.65, stadic::toDouble(radData.materials().at(1)->arg3().at(0)));
  EXPECT_EQ(.65, stadic::toDouble(radData.materials().at(1)->arg3().at(1)));
  EXPECT_EQ(.65, stadic::toDouble(radData.materials().at(1)->arg3().at(2)));

  //Testing last material for contents
  EXPECT_EQ("void", radData.materials().at(radData.materials().size()-1)->modifier());
  EXPECT_EQ(stadic::RadPrimitive::Plastic, radData.materials().at(radData.materials().size()-1)->type());
  EXPECT_EQ("l_ceiling", radData.materials().at(radData.materials().size()-1)->name());
  EXPECT_EQ(0, radData.materials().at(radData.materials().size()-1)->arg1().size());
  EXPECT_EQ(0, radData.materials().at(radData.materials().size()-1)->arg2().size());
  ASSERT_EQ(5, radData.materials().at(radData.materials().size()-1)->arg3().size());
  EXPECT_EQ(.8, stadic::toDouble(radData.materials().at(radData.materials().size()-1)->arg3().at(0)));
  EXPECT_EQ(.8, stadic::toDouble(radData.materials().at(radData.materials().size()-1)->arg3().at(1)));
  EXPECT_EQ(.8, stadic::toDouble(radData.materials().at(radData.materials().size()-1)->arg3().at(2)));
  EXPECT_EQ(0, stadic::toDouble(radData.materials().at(radData.materials().size()-1)->arg3().at(3)));
  EXPECT_EQ(0, stadic::toDouble(radData.materials().at(radData.materials().size()-1)->arg3().at(4)));

  //Test getting primitives by type
  shared_vector<stadic::PlasticMaterial> plastic = radData.getPrimitives<stadic::PlasticMaterial>();
  ASSERT_EQ(5,plastic.size());

  //Testing second plastic for contents
  EXPECT_EQ("l_extwall",plastic[1]->name());
  EXPECT_EQ(.15,plastic[1]->red());
  EXPECT_EQ(.15,plastic[1]->green());
  EXPECT_EQ(.15,plastic[1]->blue());
  EXPECT_EQ(0,plastic[1]->specularity());
  EXPECT_EQ(0,plastic[1]->roughness());
}

bool isGlass(stadic::RadPrimitive* primitive)
{
    return primitive->type() == stadic::RadPrimitive::Glass;
}
bool nameStartsWith(stadic::RadPrimitive* primitive, const std::string &name)
{
    if (primitive->name().compare(0,name.length(),name)==0){
        return true;
    }
    return false;
  //return primitive->name().startsWith(name);
}

TEST(RadFileTests, SplitRadFile)
{
    // All of the split stuff needs to be reworked, so this test will fail until that happens
    /*
  stadic::RadFileData radData;
  ASSERT_TRUE(radData.addRad("Simple.rad"));
  ASSERT_EQ(36, radData.primitives().size());
  ASSERT_EQ(36, radData.geometry().size());
  ASSERT_EQ(0, radData.materials().size());
  ASSERT_TRUE(radData.addRad("material.rad"));
  //Testing to ensure all of the primitives are read in
  ASSERT_EQ(42, radData.primitives().size());
  ASSERT_EQ(36, radData.geometry().size());
  ASSERT_EQ(6, radData.materials().size());
  // Split based on whether the type is glass or not
  QPair<stadic::RadFileData*, stadic::RadFileData*> splitGlass = radData.split(isGlass);
  ASSERT_EQ(42, radData.primitives().size());
  ASSERT_NE(nullptr, splitGlass.first);
  ASSERT_NE(nullptr, splitGlass.second);
  ASSERT_EQ(1, splitGlass.first->primitives().size());
  ASSERT_EQ(41, splitGlass.second->primitives().size());
  // Split based on what the name starts with
  QPair<stadic::RadFileData*, stadic::RadFileData*> splitName = radData.split(nameStartsWith,QString("l_wa"));
  ASSERT_EQ(42, radData.primitives().size());
  ASSERT_NE(nullptr, splitName.first);
  ASSERT_NE(nullptr, splitName.second);
  ASSERT_EQ(17, splitName.first->primitives().size());
  ASSERT_EQ(25, splitName.second->primitives().size());
  // Split based on layer names
  std::vector<QString> names;
  names.push_back("l_ceiling");
  names.push_back("l_floor");
  QPair<stadic::RadFileData*, stadic::RadFileData*> splitLayers = radData.split(names);
  ASSERT_EQ(42, radData.primitives().size());
  ASSERT_NE(nullptr, splitLayers.first);
  ASSERT_NE(nullptr, splitLayers.second);
  ASSERT_EQ(4, splitLayers.first->primitives().size());
  ASSERT_EQ(38, splitLayers.second->primitives().size());
  */
    ASSERT_TRUE(false);
}
