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
    counter=0;
    while (std::getline(iFile, line)){
        counter++;
        if (counter==10){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,' ');
            ASSERT_EQ(9,vals.size());
            EXPECT_EQ("void",vals[0]);
            EXPECT_EQ("light",vals[1]);
            EXPECT_EQ("solar10",vals[2]);
            EXPECT_EQ("0",vals[3]);
            EXPECT_EQ("0",vals[4]);
            EXPECT_EQ("3",vals[5]);
            EXPECT_EQ("1.0",vals[6]);
            EXPECT_EQ("1.0",vals[7]);
            EXPECT_EQ("1.0",vals[8]);

        }
    }
    iFile.close();
    EXPECT_EQ(1511, counter);
    iFile.open("suns.smx");
    ASSERT_TRUE(iFile.is_open());
    counter=0;
    while (std::getline(iFile, line)){
        counter++;
        if (counter==5838){
            std::vector<std::string> vals;
            vals=stadic::trimmedSplit(line,'\t');
            ASSERT_EQ(3,vals.size());
            EXPECT_EQ("1.54e+004",vals[0]);
            EXPECT_EQ("1.54e+004",vals[1]);
            EXPECT_EQ("1.54e+004",vals[2]);

        }
    }
    iFile.close();
    EXPECT_EQ(13236360, counter);



}
