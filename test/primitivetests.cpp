/****************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted for
 * personal and commercial purposes provided that the
 * following conditions are met:
 *
 * 1. Redistribution of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 2. Redistribution in binary form must reproduce the
 *    above copyright notice, this list of conditions
 *    and the following disclaimer.
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University.
 *
 * THIS SOFTWARE IS PROVIDED BY THE PENNSYLVANIA STATE UNIVERSITY
 * "AS IS" AND ANY EXPRESSED OR IMPLIED WARRANTIES, INCLUDING,
 * BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE, AND NONINFRINGEMENT OF
 * INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN NO EVENT
 * SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 * LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
 * OF SUCH DAMAGE.
 ****************************************************************/

#include "radprimitive.h"
#include "gtest/gtest.h"
#include "materialprimitives.h"
#include "geometryprimitives.h"

TEST(PrimitiveTests, Plastic)
{
    // Test no argument constructor
    stadic::PlasticMaterial noargs;
    EXPECT_EQ(0, noargs.red());
    EXPECT_EQ(0, noargs.green());
    EXPECT_EQ(0, noargs.blue());
    EXPECT_EQ(0, noargs.specularity());
    EXPECT_EQ(0, noargs.roughness());

    // Test bad arguments to constructor
    stadic::PlasticMaterial badargs(1.2, 1.8, 1.1, 1.5, 1.75);
    EXPECT_EQ(0, badargs.red());
    EXPECT_EQ(0, badargs.green());
    EXPECT_EQ(0, badargs.blue());
    EXPECT_EQ(0, badargs.specularity());
    EXPECT_EQ(0, badargs.roughness());

    // Test regular construction
    stadic::PlasticMaterial rad(0.2, 0.8, 0.1, 0.5, 0.75);
    EXPECT_EQ(stadic::RadPrimitive::Plastic, rad.type());
    EXPECT_EQ("plastic", rad.typeString());
    // Arg checks
    EXPECT_EQ(0, rad.arg1().size());
    EXPECT_EQ(0, rad.arg2().size());
    EXPECT_EQ(5, rad.arg3().size());
    EXPECT_EQ("0.2", rad.getArg3(0));
    EXPECT_EQ("0.8", rad.getArg3(1));
    EXPECT_EQ("0.1", rad.getArg3(2));
    EXPECT_EQ("0.5", rad.getArg3(3));
    EXPECT_EQ("0.75", rad.getArg3(4));
    EXPECT_FALSE(rad.setArg3("Not a number",4));
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
 
}

TEST(PrimitiveTests, Metal)
{
    // Test no argument constructor
    stadic::MetalMaterial noargs;
    EXPECT_EQ(0, noargs.red());
    EXPECT_EQ(0, noargs.green());
    EXPECT_EQ(0, noargs.blue());
    EXPECT_EQ(0, noargs.specularity());
    EXPECT_EQ(0, noargs.roughness());

    // Test bad arguments to constructor
    stadic::MetalMaterial badargs(1.2, 1.8, 1.1, 1.5, 1.75);
    EXPECT_EQ(0, badargs.red());
    EXPECT_EQ(0, badargs.green());
    EXPECT_EQ(0, badargs.blue());
    EXPECT_EQ(0, badargs.specularity());
    EXPECT_EQ(0, badargs.roughness());

    // Test regular construction
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
}

TEST(PrimitiveTests, Trans)
{
    // Test no argument constructor
    stadic::TransMaterial noargs;
    EXPECT_EQ(0, noargs.red());
    EXPECT_EQ(0, noargs.red());
    EXPECT_EQ(0, noargs.green());
    EXPECT_EQ(0, noargs.blue());
    EXPECT_EQ(0, noargs.specularity());
    EXPECT_EQ(0, noargs.roughness());
    EXPECT_EQ(0, noargs.transmissivity());
    EXPECT_EQ(0, noargs.transSpecular());

    // Test bad arguments to constructor
    stadic::TransMaterial badargs(1.2, 1.8, 1.1, 1.5, 1.75, 1.2, 1.1);
    EXPECT_EQ(0, badargs.red());
    EXPECT_EQ(0, badargs.red());
    EXPECT_EQ(0, badargs.green());
    EXPECT_EQ(0, badargs.blue());
    EXPECT_EQ(0, badargs.specularity());
    EXPECT_EQ(0, badargs.roughness());
    EXPECT_EQ(0, badargs.transmissivity());
    EXPECT_EQ(0, badargs.transSpecular());

    // Test regular construction
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
    EXPECT_EQ("0.2", rad.getArg3(5));
    EXPECT_EQ("0.1", rad.getArg3(6));
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
    
}

TEST(PrimitiveTests, Glass)
{
    // Test no argument constructor
    stadic::GlassMaterial noargs;
    EXPECT_EQ(0, noargs.redTrans());
    EXPECT_EQ(0, noargs.greenTrans());
    EXPECT_EQ(0, noargs.blueTrans());
    EXPECT_EQ(1.52, noargs.refraction());

    // Test bad arguments to constructor
    stadic::GlassMaterial badargs(1.6, 1.7, 1.5);
    EXPECT_EQ(0, badargs.redTrans());
    EXPECT_EQ(0, badargs.greenTrans());
    EXPECT_EQ(0, badargs.blueTrans());
    EXPECT_EQ(1.52, badargs.refraction());

    // Test 3 arguments to constructor
    stadic::GlassMaterial rad(0.6, 0.7, 0.5);
    EXPECT_EQ(stadic::RadPrimitive::Glass, rad.type());
    EXPECT_EQ("glass", rad.typeString());
    EXPECT_EQ(0, rad.arg1().size());
    EXPECT_EQ(0, rad.arg2().size());
    ASSERT_EQ(3, rad.arg3().size());
    EXPECT_EQ("0.6", rad.getArg3(0));
    EXPECT_EQ("0.7", rad.getArg3(1));
    EXPECT_EQ("0.5", rad.getArg3(2));

    // Expand the third argument list by one
    EXPECT_EQ(1.52, rad.refraction());
    rad.setRefraction(1.51);
    ASSERT_EQ(4, rad.arg3().size());
    EXPECT_EQ(1.51, rad.refraction());

    // Test getters and setters
    EXPECT_FALSE(rad.setType("polygon"));
    EXPECT_EQ(0.6, rad.redTrans());
    EXPECT_EQ(0.7, rad.greenTrans());
    EXPECT_EQ(0.5, rad.blueTrans());
    EXPECT_EQ(1.51, rad.refraction());
    EXPECT_FALSE(rad.setRedTrans(1.7));
    EXPECT_FALSE(rad.setGreenTrans(-300));
    EXPECT_FALSE(rad.setBlueTrans(4000));
    EXPECT_EQ(0.6, rad.redTrans());
    EXPECT_EQ(0.7, rad.greenTrans());
    EXPECT_EQ(0.5, rad.blueTrans());

    // Test 4 arguments to constructor
    stadic::GlassMaterial rad4(0.6, 0.7, 0.5, 1.75);
    EXPECT_EQ(stadic::RadPrimitive::Glass, rad4.type());
    EXPECT_EQ("glass", rad4.typeString());
    EXPECT_EQ(0, rad4.arg1().size());
    EXPECT_EQ(0, rad4.arg2().size());
    ASSERT_EQ(4, rad4.arg3().size());
    EXPECT_EQ("0.6", rad4.getArg3(0));
    EXPECT_EQ("0.7", rad4.getArg3(1));
    EXPECT_EQ("0.5", rad4.getArg3(2));
    EXPECT_EQ("1.75", rad4.getArg3(3));
    EXPECT_EQ(0.6, rad4.redTrans());
    EXPECT_EQ(0.7, rad4.greenTrans());
    EXPECT_EQ(0.5, rad4.blueTrans());
    EXPECT_EQ(1.75, rad4.refraction());
    EXPECT_FALSE(rad4.setRefraction(-1000));
}

TEST(PrimitiveTests, BSDF)
{
    // Test no argument constructor
    stadic::BSDFMaterial noargs;
    EXPECT_EQ(0, noargs.thickness());
    EXPECT_EQ("null", noargs.bsdfFile());
    EXPECT_EQ(0, noargs.ux());
    EXPECT_EQ(0, noargs.uy());
    EXPECT_EQ(0, noargs.uz());

    stadic::BSDFMaterial rad(0,"bsdf.xml", 0,-1,0);
    EXPECT_EQ(stadic::RadPrimitive::BSDF, rad.type());
    EXPECT_EQ("bsdf", rad.typeString());
    // Arg checks
    EXPECT_EQ(6, rad.arg1().size());
    EXPECT_EQ(0, rad.arg2().size());
    ASSERT_EQ(0, rad.arg3().size());
    EXPECT_EQ("0", rad.getArg1(0));
    EXPECT_EQ("bsdf.xml", rad.getArg1(1));
    EXPECT_EQ("0", rad.getArg1(2));
    EXPECT_EQ("-1", rad.getArg1(3));
    EXPECT_EQ("0", rad.getArg1(4));
    EXPECT_EQ(".", rad.getArg1(5));
    // Miscellaneous checks
    EXPECT_FALSE(rad.setType("polygon"));
    EXPECT_EQ(0, rad.thickness());
    EXPECT_EQ("bsdf.xml", rad.bsdfFile());
    EXPECT_EQ(0, rad.ux());
    EXPECT_EQ(-1, rad.uy());
    EXPECT_EQ(0, rad.uz());
}


TEST(PrimitiveTests, Polygon)
{
    std::vector<double> points;
    //Polygon with points at (0,0,0);(120,0,0);(120,120,0);(0,120,0)
    points.push_back(0);
    points.push_back(0);
    points.push_back(0);
    points.push_back(120);
    points.push_back(0);
    points.push_back(0);
    points.push_back(120);
    points.push_back(120);
    points.push_back(0);
    points.push_back(0);
    points.push_back(120);
    points.push_back(0);
    stadic::PolygonGeometry rad(points);
    EXPECT_EQ(stadic::RadPrimitive::Polygon, rad.type());
    EXPECT_EQ("polygon", rad.typeString());
    // Arg checks
    EXPECT_EQ(0, rad.arg1().size());
    EXPECT_EQ(0, rad.arg2().size());
    ASSERT_EQ(12, rad.arg3().size());
    EXPECT_EQ("0", rad.getArg3(0));
    EXPECT_EQ("0", rad.getArg3(1));
    EXPECT_EQ("0", rad.getArg3(2));
    EXPECT_EQ("120", rad.getArg3(3));
    EXPECT_EQ("0", rad.getArg3(4));
    EXPECT_EQ("0", rad.getArg3(5));
    EXPECT_EQ("120", rad.getArg3(6));
    EXPECT_EQ("120", rad.getArg3(7));
    EXPECT_EQ("0", rad.getArg3(8));
    EXPECT_EQ("0", rad.getArg3(9));
    EXPECT_EQ("120", rad.getArg3(10));
    EXPECT_EQ("0", rad.getArg3(11));
    // Miscellaneous checks
    EXPECT_FALSE(rad.setType("sphere"));
    ASSERT_EQ(12, rad.points().size());
    EXPECT_EQ(0, rad.points()[0]);
    EXPECT_EQ(0, rad.points()[1]);
    EXPECT_EQ(0, rad.points()[2]);
    EXPECT_EQ(120, rad.points()[3]);
    EXPECT_EQ(0, rad.points()[4]);
    EXPECT_EQ(0, rad.points()[5]);
    EXPECT_EQ(120, rad.points()[6]);
    EXPECT_EQ(120, rad.points()[7]);
    EXPECT_EQ(0, rad.points()[8]);
    EXPECT_EQ(0, rad.points()[9]);
    EXPECT_EQ(120, rad.points()[10]);
    EXPECT_EQ(0, rad.points()[11]);
}

TEST(PrimitiveTests, Sphere)
{
    std::vector<double> centerPoint;
    centerPoint.push_back(0);
    centerPoint.push_back(0);
    centerPoint.push_back(0);
    stadic::SphereGeometry rad(centerPoint, 3);
    EXPECT_EQ(stadic::RadPrimitive::Sphere, rad.type());
    EXPECT_EQ("sphere", rad.typeString());
    // Arg checks
    EXPECT_EQ(0, rad.arg1().size());
    EXPECT_EQ(0, rad.arg2().size());
    ASSERT_EQ(4, rad.arg3().size());
    EXPECT_EQ("0", rad.getArg3(0));
    EXPECT_EQ("0", rad.getArg3(1));
    EXPECT_EQ("0", rad.getArg3(2));
    EXPECT_EQ("3", rad.getArg3(3));
    // Miscellaneous checks
    EXPECT_FALSE(rad.setType("polygon"));
    ASSERT_EQ(3, rad.centerPoint().size());
    EXPECT_EQ(0, rad.centerPoint()[0]);
    EXPECT_EQ(0, rad.centerPoint()[1]);
    EXPECT_EQ(0, rad.centerPoint()[2]);
    EXPECT_EQ(3, rad.radius());
}

TEST(PrimitiveTests, Ring)
{
    std::vector<double> centerPoint;
    centerPoint.push_back(0);
    centerPoint.push_back(0);
    centerPoint.push_back(0);
    std::vector<double> surfaceNormal;
    surfaceNormal.push_back(0);
    surfaceNormal.push_back(0);
    surfaceNormal.push_back(1);
    stadic::RingGeometry rad(centerPoint, surfaceNormal, 0, 6);
    EXPECT_EQ(stadic::RadPrimitive::Ring, rad.type());
    EXPECT_EQ("ring", rad.typeString());
    // Arg checks
    EXPECT_EQ(0, rad.arg1().size());
    EXPECT_EQ(0, rad.arg2().size());
    ASSERT_EQ(8, rad.arg3().size());
    EXPECT_EQ("0", rad.getArg3(0));
    EXPECT_EQ("0", rad.getArg3(1));
    EXPECT_EQ("0", rad.getArg3(2));
    EXPECT_EQ("0", rad.getArg3(3));
    EXPECT_EQ("0", rad.getArg3(4));
    EXPECT_EQ("1", rad.getArg3(5));
    EXPECT_EQ("0", rad.getArg3(6));
    EXPECT_EQ("6", rad.getArg3(7));
    // Miscellaneous checks
    EXPECT_FALSE(rad.setType("polygon"));
    ASSERT_EQ(3, rad.centerPoint().size());
    EXPECT_EQ(0, rad.centerPoint()[0]);
    EXPECT_EQ(0, rad.centerPoint()[1]);
    EXPECT_EQ(0, rad.centerPoint()[2]);
    ASSERT_EQ(3, rad.surfaceNormal().size());
    EXPECT_EQ(0, rad.surfaceNormal()[0]);
    EXPECT_EQ(0, rad.surfaceNormal()[1]);
    EXPECT_EQ(1, rad.surfaceNormal()[2]);
    EXPECT_EQ(0, rad.innerRadius());
    EXPECT_EQ(6, rad.outerRadius());
}

