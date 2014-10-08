#include "gridmaker.h"
#include "gtest/gtest.h"
#include <fstream>
#include <string>
#include "functions.h"
#include <vector>


TEST(GridTests, SimpleInset){
    std::vector<std::string> files;
    files.clear();
    files.push_back("simple.rad");
    stadic::GridMaker grid(files);

    std::vector<std::string> layers;
    layers.clear();
    layers.push_back("l_floor");
    grid.setLayerNames(layers);
    grid.setOffset(12);
    grid.setSpaceX(24);
    grid.setSpaceY(24);
    grid.setOffsetZ(30);
    ASSERT_TRUE(grid.makeGrid());
}

TEST(GridTests, SimpleOffset){
    std::vector<std::string> files;
    files.clear();
    files.push_back("simple.rad");
    stadic::GridMaker grid(files);

    std::vector<std::string> layers;
    layers.clear();
    layers.push_back("l_floor");
    grid.setLayerNames(layers);
    grid.setOffsetX(24);
    grid.setOffsetY(24);
    grid.setSpaceX(24);
    grid.setSpaceY(24);
    grid.setOffsetZ(30);
    ASSERT_TRUE(grid.makeGrid());
}

TEST(GridTests, Complicated)
{
    std::vector<std::string> files;
    files.clear();
    files.push_back("complicated.rad");
    stadic::GridMaker grid(files);

    std::vector<std::string> layers;
    layers.clear();
    layers.push_back("l_groundfloor");
    layers.push_back("l_firstfloor");
    layers.push_back("l_secondfloor");
    layers.push_back("l_stairs");
    grid.setLayerNames(layers);
    grid.setOffset(24);
    grid.setSpaceX(24);
    grid.setSpaceY(24);
    grid.setOffsetZ(30);
    ASSERT_TRUE(grid.makeGrid());
    ASSERT_TRUE(grid.writePTS("complicatedgrid.pts"));
    ASSERT_TRUE(grid.viewPTS("", "se"));
    std::ifstream iFile;
    iFile.open("complicatedgrid.pts");
    ASSERT_TRUE(iFile.is_open());
    std::string line;
    int counter=0;
    while (std::getline(iFile, line)){
        counter++;
        if (counter==1){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("24",vals[0]);
            EXPECT_EQ("24",vals[1]);
            EXPECT_EQ("30",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==58){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("48",vals[0]);
            EXPECT_EQ("696",vals[1]);
            EXPECT_EQ("30",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==1131){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("936",vals[0]);
            EXPECT_EQ("696",vals[1]);
            EXPECT_EQ("30",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==1132){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("24.3022",vals[0]);
            EXPECT_EQ("441.087",vals[1]);
            EXPECT_EQ("104",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==1359){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("936",vals[0]);
            EXPECT_EQ("691.105",vals[1]);
            EXPECT_EQ("174",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==1554){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("936",vals[0]);
            EXPECT_EQ("684.087",vals[1]);
            EXPECT_EQ("318",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
    }
    iFile.close();
    EXPECT_EQ(1554, counter);
}
