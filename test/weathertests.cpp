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
 *    and the following disclaimer
 *
 * 3. Neither the name of The Pennsylvania State University
 *    nor the names of its contributors may be used to
 *    endorse or promote products derived from this software
 *    without the specific prior written permission of The
 *    Pennsylvania State University
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

