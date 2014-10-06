#include "radprimitive.h"
#include "gtest/gtest.h"
#include "materialprimitives.h"

TEST(PrimitiveTests, Plastic)
{
  stadic::PlasticMaterial rad(0.2,0.8,0.1,0.5,0.75);
  EXPECT_EQ(stadic::RadPrimitive::Plastic,rad.type());
  EXPECT_EQ("plastic",rad.typeString());
  // Arg checks
  EXPECT_EQ(0,rad.arg1().size());
  EXPECT_EQ(0,rad.arg2().size());
  EXPECT_EQ(5,rad.arg3().size());
  EXPECT_EQ("0.2",rad.getArg3(0));
  EXPECT_EQ("0.8",rad.getArg3(1));
  EXPECT_EQ("0.1",rad.getArg3(2));
  EXPECT_EQ("0.5",rad.getArg3(3));
  EXPECT_EQ("0.75",rad.getArg3(4));
  EXPECT_EQ("0.2",rad.getArg(3,0));
  EXPECT_EQ("0.8",rad.getArg(3,1));
  EXPECT_EQ("0.1",rad.getArg(3,2));
  EXPECT_EQ("0.5",rad.getArg(3,3));
  EXPECT_EQ("0.75",rad.getArg(3,4));
  // Miscellaneous checks
  EXPECT_FALSE(rad.setType("polygon"));
  EXPECT_FALSE(rad.setRed(250));
  EXPECT_FALSE(rad.setGreen(20));
  EXPECT_FALSE(rad.setBlue(-250));
  EXPECT_EQ(0.2,rad.red());
  EXPECT_EQ(0.8,rad.green());
  EXPECT_EQ(0.1,rad.blue());
  EXPECT_EQ(0.5,rad.specularity());
  EXPECT_EQ(0.75,rad.roughness());
}

TEST(PrimitiveTests, Metal)
{
    stadic::MetalMaterial rad(0.2, 0.8, 0.1, 0.5, 0.75);
    EXPECT_EQ(stadic::RadPrimitive::Metal, rad.type());
    EXPECT_EQ("metal", rad.typeString());
    // Arg checks
    EXPECT_EQ(0, rad.arg1().size());
    EXPECT_EQ(0, rad.arg2().size());
    EXPECT_EQ(5, rad.arg3().size());
    EXPECT_EQ("0.2", rad.getArg3(0));
    EXPECT_EQ("0.8", rad.getArg3(1));
    EXPECT_EQ("0.1", rad.getArg3(2));
    EXPECT_EQ("0.5", rad.getArg3(3));
    EXPECT_EQ("0.75", rad.getArg3(4));
    EXPECT_EQ("0.2", rad.getArg(3, 0));
    EXPECT_EQ("0.8", rad.getArg(3, 1));
    EXPECT_EQ("0.1", rad.getArg(3, 2));
    EXPECT_EQ("0.5", rad.getArg(3, 3));
    EXPECT_EQ("0.75", rad.getArg(3, 4));
    // Miscellaneous checks
    EXPECT_FALSE(rad.setType("polygon"));
    EXPECT_FALSE(rad.setRed(250));
    EXPECT_FALSE(rad.setGreen(20));
    EXPECT_FALSE(rad.setBlue(-250));
    EXPECT_EQ(0.2, rad.red());
    EXPECT_EQ(0.8, rad.green());
    EXPECT_EQ(0.1, rad.blue());
    EXPECT_EQ(0.5, rad.specularity());
    EXPECT_EQ(0.75, rad.roughness());
    // Bad constructor args
    stadic::MetalMaterial rad2(1.2, 0.8, 0.1, 0.5, 0.75);
    EXPECT_EQ(0, rad2.red());
    EXPECT_EQ(0.8, rad2.green());
}

TEST(PrimitiveTests, Trans)
{
    stadic::TransMaterial rad(0.2, 0.8, 0.1, 0.5, 0.75, 0.2, 0.1);
    EXPECT_EQ(stadic::RadPrimitive::Trans, rad.type());
    EXPECT_EQ("trans", rad.typeString());
    // Arg checks
    EXPECT_EQ(0, rad.arg1().size());
    EXPECT_EQ(0, rad.arg2().size());
    EXPECT_EQ(7, rad.arg3().size());
    EXPECT_EQ("0.2", rad.getArg3(0));
    EXPECT_EQ("0.8", rad.getArg3(1));
    EXPECT_EQ("0.1", rad.getArg3(2));
    EXPECT_EQ("0.5", rad.getArg3(3));
    EXPECT_EQ("0.75", rad.getArg3(4));
    EXPECT_EQ("0.2", rad.getArg(3, 0));
    EXPECT_EQ("0.8", rad.getArg(3, 1));
    EXPECT_EQ("0.1", rad.getArg(3, 2));
    EXPECT_EQ("0.5", rad.getArg(3, 3));
    EXPECT_EQ("0.75", rad.getArg(3, 4));
    EXPECT_EQ("0.2", rad.getArg(3, 5));
    EXPECT_EQ("0.1", rad.getArg(3, 6));
    // Miscellaneous checks
    EXPECT_FALSE(rad.setType("polygon"));
    EXPECT_FALSE(rad.setRed(250));
    EXPECT_FALSE(rad.setGreen(20));
    EXPECT_FALSE(rad.setBlue(-250));
    EXPECT_EQ(0.2, rad.red());
    EXPECT_EQ(0.8, rad.green());
    EXPECT_EQ(0.1, rad.blue());
    EXPECT_EQ(0.5, rad.specularity());
    EXPECT_EQ(0.75, rad.roughness());
    EXPECT_EQ(0.2, rad.transmissivity());
    EXPECT_EQ(0.1, rad.transSpecular());
    // Bad constructor args
    stadic::TransMaterial rad2(1.2, 1.8, 1.1, 1.5, 1.75, 1.2, 1.1);
    EXPECT_EQ(0, rad2.red());
    EXPECT_EQ(0, rad2.red());
    EXPECT_EQ(0, rad2.green());
    EXPECT_EQ(0, rad2.blue());
    EXPECT_EQ(0, rad2.specularity());
    EXPECT_EQ(0, rad2.roughness());
    EXPECT_EQ(0, rad2.transmissivity());
    EXPECT_EQ(0, rad2.transSpecular());
}