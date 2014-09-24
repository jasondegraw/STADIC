#include "weatherdata.h"
#include "gtest/gtest.h"

TEST(WeatherTests, ReadEpw)
{
  stadic::WeatherData data;
  ASSERT_TRUE(data.parseWeather("USA_PA_Lancaster.AP.725116_TMY3.epw"));
  // Check location
  EXPECT_EQ("Lancaster", data.place());
  EXPECT_EQ("40.12", data.latitude());
  EXPECT_EQ("-76.30", data.longitude());
  EXPECT_EQ("-5.0", data.timeZone());
  EXPECT_EQ("126.0", data.elevation());
  ASSERT_EQ(8760,data.month().size());
  // Spot check of data
  EXPECT_EQ(1,data.month()[710]);
  EXPECT_EQ(30,data.day()[710]);
  EXPECT_EQ(14.5,data.hour()[710]);
  EXPECT_EQ("0",data.directNormal()[710]);
  EXPECT_EQ("67",data.diffuseHorizontal()[710]);
}

TEST(WeatherTests, WriteWea)
{

}
