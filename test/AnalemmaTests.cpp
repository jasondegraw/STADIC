#include "analemma.h"
#include "gtest/gtest.h"
#include <fstream>
#include <string>
#include "functions.h"
#include <vector>

TEST(AnalemmaTests, TestLancaster)
{
    stadic::Analemma suns("USA_PA_Lancaster.AP.725116_TMY3.epw");
    suns.setGeoFile("sunsGeo.rad");
    suns.setMatFile("sunsMat.rad");
    suns.setRotation(0);
    suns.setSMXFile("suns.smx");
    ASSERT_TRUE(suns.genSun());
    std::fstream iFile;
    iFile.open("sunsGeo.rad");
    ASSERT_TRUE(iFile.is_open());
    std::string line;
    int counter=0;
    while (std::getline(iFile, line)){
        counter++;
        if (counter==10){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(10,vals.size());
            EXPECT_EQ("solar10",vals[0]);
            EXPECT_EQ("source",vals[1]);
            EXPECT_EQ("sun10",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("4",vals[5]);
            EXPECT_EQ("0.963768",vals[6]);
            EXPECT_EQ("0.250951",vals[7]);
            EXPECT_EQ("0.0904094",vals[8]);
            EXPECT_EQ("0.533",vals[9]);
        }
    }
    iFile.close();
    EXPECT_EQ(1511, counter);
    //EXPECT_NEAR(1500,counter,15);
    iFile.open("sunsMat.rad");
    ASSERT_TRUE(iFile.is_open());
    while (std::getline(iFile, line)){
        counter++;
        if (counter==10){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');

        }
    }
    iFile.close();



}
