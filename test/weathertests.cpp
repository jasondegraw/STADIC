#include "makewea.h"
#include "gtest/gtest.h"

TEST(WeatherTests, ReadEpw)
{
  stadic::WeatherData data;
  data.parseWeather(":/resources/USA_PA_Lancaster.AP.725116_TMY3.epw");
  // Check location
  EXPECT_EQ("Lancaster", data.place().toStdString());
  EXPECT_EQ("40.12", data.latitude().toStdString());
  EXPECT_EQ("-76.30", data.longitude().toStdString());
  EXPECT_EQ("-5.0", data.timeZone().toStdString());
  EXPECT_EQ("126.0", data.elevation().toStdString());
  ASSERT_EQ(8760,data.month().size());
  // Spot check of data
  //EXPECT_EQ("",data.month()[710].toStdString());
  //EXPECT_EQ("",data.day()[710].toStdString());
  EXPECT_EQ(14.5,data.hour()[710]);
  EXPECT_EQ("0",data.directNormal()[710].toStdString());
  EXPECT_EQ("67",data.directHorizontal()[710].toStdString());
}

TEST(WeatherTests, WriteWea)
{

}
