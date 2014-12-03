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

TEST(GridTests, ComplicatedThreshold)
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
    grid.setThreshold(73);      //This line prevents the stairs (@72" long) from being included in the points generation
    ASSERT_TRUE(grid.makeGrid());
    ASSERT_TRUE(grid.writePTS("complicatedgrid1.pts"));
    std::ifstream iFile;
    iFile.open("complicatedgrid1.pts");
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
            EXPECT_EQ("21.9072",vals[0]);
            EXPECT_EQ("438.692",vals[1]);
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

TEST(GridTests, ComplicatedNoStairs)
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
    grid.setLayerNames(layers);
    grid.setOffset(24);
    grid.setSpaceX(24);
    grid.setSpaceY(24);
    grid.setOffsetZ(30);
    ASSERT_TRUE(grid.makeGrid());
    ASSERT_TRUE(grid.writePTS("complicatedgrid2.pts"));
    std::ifstream iFile;
    iFile.open("complicatedgrid2.pts");
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
            EXPECT_EQ("24",vals[0]);
            EXPECT_EQ("283.105",vals[1]);
            EXPECT_EQ("174",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==1359){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("24",vals[0]);
            EXPECT_EQ("660.087",vals[1]);
            EXPECT_EQ("318",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==1550){
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
    EXPECT_EQ(1550, counter);
}


TEST(GridTests, ComplicatedNoThreshold)
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
    ASSERT_TRUE(grid.writePTS("complicatedgrid3.pts"));
    ASSERT_TRUE(grid.viewPTS("", "se"));
    std::ifstream iFile;
    iFile.open("complicatedgrid3.pts");
    ASSERT_TRUE(iFile.is_open());
    std::string line;
    int counter=0;
    while (std::getline(iFile, line)){
        counter++;
        if (counter==1){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("5.79722",vals[0]);
            EXPECT_EQ("558.582",vals[1]);
            EXPECT_EQ("167",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==58){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("48",vals[0]);
            EXPECT_EQ("624",vals[1]);
            EXPECT_EQ("30",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==1134){
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
        if (counter==1135){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("5.79722",vals[0]);
            EXPECT_EQ("547.582",vals[1]);
            EXPECT_EQ("160",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==1359){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("408",vals[0]);
            EXPECT_EQ("595.105",vals[1]);
            EXPECT_EQ("174",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
        if (counter==1668){
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
    EXPECT_EQ(1668, counter);
}
TEST(GridTests, Diamond)
{
    std::vector<std::string> files;
    files.clear();
    files.push_back("diamond.rad");
    stadic::GridMaker grid(files);
    std::vector<std::string> layers;
    layers.clear();
    layers.push_back("floor");
    grid.setLayerNames(layers);
    grid.setOffset(1);
    grid.setSpaceX(1);
    grid.setSpaceY(1);
    grid.setOffsetZ(2.5);
    grid.setRotation(45);
    ASSERT_TRUE(grid.makeGrid());
    ASSERT_TRUE(grid.writePTS("diamondgrid.pts"));
    ASSERT_TRUE(grid.viewPTS("", "p"));
    std::ifstream iFile;
    iFile.open("diamondgrid.pts");
    ASSERT_TRUE(iFile.is_open());
    std::string line;
    int counter=0;
    while (std::getline(iFile, line)){
        counter++;
        if (counter==1){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("10",vals[0]);
            EXPECT_EQ("1.41421",vals[1]);
            EXPECT_EQ("2.5",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }else if (counter==7){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("10.7071",vals[0]);
            EXPECT_EQ("2.12132",vals[1]);
            EXPECT_EQ("2.5",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }else if (counter==36){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(6,vals.size());
            EXPECT_EQ("10",vals[0]);
            EXPECT_EQ("8.48528",vals[1]);
            EXPECT_EQ("2.5",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("1",vals[5]);
        }
    }
    iFile.close();
    EXPECT_EQ(36, counter);
}
