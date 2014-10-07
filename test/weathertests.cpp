#include "weatherdata.h"
#include "gtest/gtest.h"
#include <fstream>
#include <string>
#include "functions.h"

TEST(WeatherTests, ReadEpw)
{
  stadic::WeatherData data;
  ASSERT_TRUE(data.parseWeather("USA_PA_Lancaster.AP.725116_TMY3.epw"));
  // Check location
  EXPECT_EQ("Lancaster_USA", data.place());
  EXPECT_EQ("40.12", data.latitude());
  EXPECT_EQ("76.3", data.longitude());
  EXPECT_EQ("5", data.timeZone());
  EXPECT_EQ("126.0", data.elevation());
  ASSERT_EQ(8760,data.month().size());
  // Spot check of data
  EXPECT_EQ(1,data.month()[710]);
  EXPECT_EQ(30,data.day()[710]);
  EXPECT_EQ(14.5,data.hour()[710]);
  EXPECT_EQ("0",data.directNormal()[710]);
  EXPECT_EQ("67",data.diffuseHorizontal()[710]);
}

TEST(WeatherTests, ReadTMY)
{
  stadic::WeatherData data;
  ASSERT_TRUE(data.parseWeather("LancasterTMY.csv"));
  // Check location
  EXPECT_EQ("LANCASTER", data.place());
  EXPECT_EQ("40.117", data.latitude());
  EXPECT_EQ("76.3", data.longitude());
  EXPECT_EQ("5", data.timeZone());
  EXPECT_EQ("126", data.elevation());
  ASSERT_EQ(8760,data.month().size());
  // Spot check of data
  EXPECT_EQ(1,data.month()[710]);
  EXPECT_EQ(30,data.day()[710]);
  EXPECT_EQ(14.5,data.hour()[710]);
  EXPECT_EQ("0",data.directNormal()[710]);
  EXPECT_EQ("67",data.diffuseHorizontal()[710]);
}


TEST(WeatherTests, WriteEPWWea)
{
    stadic::WeatherData data;
    ASSERT_TRUE(data.parseWeather("USA_PA_Lancaster.AP.725116_TMY3.epw"));
    ASSERT_TRUE(data.writeWea("Lancaster.wea"));
    std::fstream iFile;
    iFile.open("Lancaster.wea");
    ASSERT_TRUE(iFile.is_open());
    std::string line;
    iFile>>line;
    EXPECT_EQ("place", line);
    iFile>>line;
    EXPECT_EQ("Lancaster_USA", line);
    iFile>>line;
    EXPECT_EQ("latitude", line);
    iFile>>line;
    EXPECT_EQ("40.12", line);
    iFile>>line;
    EXPECT_EQ("longitude", line);
    iFile>>line;
    EXPECT_EQ("76.3", line);
    iFile>>line;
    EXPECT_EQ("time_zone", line);
    iFile>>line;
    EXPECT_EQ("75", line);
    iFile>>line;
    EXPECT_EQ("site_elevation", line);
    iFile>>line;
    EXPECT_EQ("126.0", line);
    iFile>>line;
    EXPECT_EQ("weather_data_file_units", line);
    iFile>>line;
    EXPECT_EQ("1", line);
    iFile>>line;
    int counter=0;
    while (std::getline(iFile, line)){
        counter++;
        if (counter==10){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(5,vals.size());
            EXPECT_EQ("1",vals[0]);
            EXPECT_EQ("1",vals[1]);
            EXPECT_EQ("9.5",vals[2]);
            EXPECT_EQ("599",vals[3]);
            EXPECT_EQ("86",vals[4]);
        }
    }
    iFile.close();
    EXPECT_EQ(8760, counter);

}

TEST(WeatherTests, WriteTMYWea)
{
    stadic::WeatherData data;
    ASSERT_TRUE(data.parseWeather("LancasterTMY.csv"));
    ASSERT_TRUE(data.writeWea("Lancaster.wea"));
    std::fstream iFile;
    iFile.open("Lancaster.wea");
    ASSERT_TRUE(iFile.is_open());
    std::string line;
    iFile>>line;
    EXPECT_EQ("place", line);
    iFile>>line;
    EXPECT_EQ("LANCASTER", line);
    iFile>>line;
    EXPECT_EQ("latitude", line);
    iFile>>line;
    EXPECT_EQ("40.117", line);
    iFile>>line;
    EXPECT_EQ("longitude", line);
    iFile>>line;
    EXPECT_EQ("76.3", line);
    iFile>>line;
    EXPECT_EQ("time_zone", line);
    iFile>>line;
    EXPECT_EQ("75", line);
    iFile>>line;
    EXPECT_EQ("site_elevation", line);
    iFile>>line;
    EXPECT_EQ("126", line);
    iFile>>line;
    EXPECT_EQ("weather_data_file_units", line);
    iFile>>line;
    EXPECT_EQ("1", line);
    iFile>>line;
    int counter=0;
    while (std::getline(iFile, line)){
        counter++;
        if (counter==10){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(5,vals.size());
            EXPECT_EQ("1",vals[0]);
            EXPECT_EQ("1",vals[1]);
            EXPECT_EQ("9.5",vals[2]);
            EXPECT_EQ("599",vals[3]);
            EXPECT_EQ("86",vals[4]);
        }
    }
    iFile.close();
    EXPECT_EQ(8760, counter);

}

