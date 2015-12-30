/******************************************************************************
 * Copyright (c) 2014-2015, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived from
 *    this software without specific prior written permission of the
 *    respective copyright holder or contributor.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
 * AND NONINFRINGEMENT OF INTELLECTUAL PROPERTY ARE EXPRESSLY DISCLAIMED. IN
 * NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *****************************************************************************/

#include "dayill.h"
#include "gtest/gtest.h"

TEST(IlluminanceTests, ReadWriteTimeBased)
{

  stadic::DaylightIlluminanceData testIll("lux");
  ASSERT_TRUE(testIll.parseTimeBased("10PointsTimeBased.ill"));
  EXPECT_EQ(testIll.illuminance()[0].lux()[0],0);
  EXPECT_EQ(testIll.illuminance()[0].lux()[9],0);
  EXPECT_EQ(testIll.illuminance()[0].lux().size(),10);
  EXPECT_EQ(testIll.illuminance()[8].lux()[0],252);
  EXPECT_EQ(testIll.illuminance()[8].lux()[9],748);
  EXPECT_EQ(testIll.illuminance()[8].lux().size(),10);
  EXPECT_EQ(testIll.illuminance()[16].lux()[0],118);
  EXPECT_EQ(testIll.illuminance()[16].lux()[9],103);
  EXPECT_EQ(testIll.illuminance()[16].lux().size(),10);
  EXPECT_EQ(testIll.illuminance()[17].lux()[0],0);
  EXPECT_EQ(testIll.illuminance()[17].lux()[9],0);
  EXPECT_EQ(testIll.illuminance()[17].lux().size(),10);
  EXPECT_EQ(testIll.illuminance()[23].lux()[0],0);
  EXPECT_EQ(testIll.illuminance()[23].lux()[9],0);
  EXPECT_EQ(testIll.illuminance()[17].lux().size(),10);

  ASSERT_TRUE(testIll.writeIllFileLux("10PointsTestLux.ill"));
  ASSERT_TRUE(testIll.writeIllFileFC("10PointsTestFC.ill"));
  stadic::DaylightIlluminanceData testIllFC("fc");
  ASSERT_TRUE(testIllFC.parseTimeBased("10PointsTestFC.ill"));
  EXPECT_EQ(testIllFC.illuminance()[0].lux()[0],0);
  EXPECT_EQ(testIllFC.illuminance()[0].lux()[9],0);
  EXPECT_EQ(testIllFC.illuminance()[0].lux().size(),10);
  EXPECT_NEAR(testIllFC.illuminance()[8].lux()[0],252,1);
  EXPECT_NEAR(testIllFC.illuminance()[8].lux()[9],748,1);
  EXPECT_EQ(testIllFC.illuminance()[8].lux().size(),10);
  EXPECT_NEAR(testIllFC.illuminance()[16].lux()[0],118,1);
  EXPECT_NEAR(testIllFC.illuminance()[16].lux()[9],103,1);
  EXPECT_EQ(testIllFC.illuminance()[16].lux().size(),10);
  EXPECT_EQ(testIllFC.illuminance()[17].lux()[0],0);
  EXPECT_EQ(testIllFC.illuminance()[17].lux()[9],0);
  EXPECT_EQ(testIllFC.illuminance()[17].lux().size(),10);
  EXPECT_EQ(testIllFC.illuminance()[23].lux()[0],0);
  EXPECT_EQ(testIllFC.illuminance()[23].lux()[9],0);
  EXPECT_EQ(testIllFC.illuminance()[17].lux().size(),10);
}
/*
TEST(SensorTests, ReadWriteNonTimeBased)
{

  stadic::Photosensor sensor;
  ASSERT_TRUE(sensor.setType("cosine_squared"));
  ASSERT_TRUE(sensor.writeSensorFile("cosineSquared.sen"));

}
*/
