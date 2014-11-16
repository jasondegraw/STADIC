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

#include "stadiccontrol.h"
#include "gtest/gtest.h"
#include <iostream>

TEST(ControlTests, PTreeReadJson)
{
  stadic::Control controlFile;
  ASSERT_TRUE(controlFile.parseJson("TestSControl.json"));
  EXPECT_EQ("prj1",controlFile.projectName());
  EXPECT_EQ("c:/example/", controlFile.projectFolder());
  EXPECT_EQ("tmp/", controlFile.tmpFolder());
  EXPECT_EQ("rad/", controlFile.geoFolder());
  EXPECT_EQ("ies/", controlFile.iesFolder());
  EXPECT_EQ("res/", controlFile.resultsFolder());
  EXPECT_EQ("data/", controlFile.dataFolder());
  EXPECT_EQ(0.2, controlFile.groundReflect());
  EXPECT_EQ("c:/weaData/state_college.wea", controlFile.weaDataFile());
  EXPECT_EQ(1, controlFile.firstDay());
  EXPECT_EQ("main_mat.rad", controlFile.matFile());
  EXPECT_EQ("main_geo.rad", controlFile.geoFile());
  EXPECT_EQ(0, controlFile.buildingRotation());
  EXPECT_EQ("calc_grid.pts", controlFile.ptsFile());
  EXPECT_EQ(2,controlFile.windowGroups().size());

    //Window Group 1
    EXPECT_EQ("WG1", controlFile.windowGroups()[0].name());
    EXPECT_EQ(2, controlFile.windowGroups()[0].shadeSettingGeometry().size());
    EXPECT_EQ("wg1base.rad", controlFile.windowGroups()[0].baseGeometry());
    EXPECT_EQ("l_glazing", controlFile.windowGroups()[0].glazingLayers()[0]);
    EXPECT_EQ("l_glazing2", controlFile.windowGroups()[0].glazingLayers()[1]);
    EXPECT_EQ("automated_profile_angle", controlFile.windowGroups()[0].shadeControl()->controlMethod());
    EXPECT_EQ(30, controlFile.windowGroups()[0].shadeControl()->elevationAzimuth());
    EXPECT_EQ(30, controlFile.windowGroups()[0].shadeControl()->angleSettings()[0]);
    EXPECT_EQ(60, controlFile.windowGroups()[0].shadeControl()->angleSettings()[1]);
    EXPECT_EQ("wg1set1.rad", controlFile.windowGroups()[0].shadeSettingGeometry()[0]);
    EXPECT_EQ("wg1set2.rad", controlFile.windowGroups()[0].shadeSettingGeometry()[1]);

    //Window Group 2
    EXPECT_EQ("WG2", controlFile.windowGroups()[1].name());
    EXPECT_EQ(2, controlFile.windowGroups()[1].shadeSettingGeometry().size());
    EXPECT_EQ(true, controlFile.windowGroups()[1].isBSDF());
    EXPECT_EQ("wg2base.rad", controlFile.windowGroups()[1].baseGeometry());
    EXPECT_EQ("l_window", controlFile.windowGroups()[1].glazingLayers()[0]);
    EXPECT_EQ("l_window", controlFile.windowGroups()[1].bsdfBaseLayers()[0]);

    EXPECT_EQ("automated_signal", controlFile.windowGroups()[1].shadeControl()->controlMethod());
    EXPECT_EQ("sensitivity_file", controlFile.windowGroups()[1].shadeControl()->sensorType());
    EXPECT_EQ("cos.sen", controlFile.windowGroups()[1].shadeControl()->sensorFile());
    EXPECT_EQ(4, controlFile.windowGroups()[1].shadeControl()->location()[0]);
    EXPECT_EQ(0, controlFile.windowGroups()[1].shadeControl()->location()[1]);
    EXPECT_EQ(5, controlFile.windowGroups()[1].shadeControl()->location()[2]);
    EXPECT_EQ(0, controlFile.windowGroups()[1].shadeControl()->location()[3]);
    EXPECT_EQ(-1, controlFile.windowGroups()[1].shadeControl()->location()[4]);
    EXPECT_EQ(0, controlFile.windowGroups()[1].shadeControl()->location()[5]);
    EXPECT_EQ(0, controlFile.windowGroups()[1].shadeControl()->location()[6]);
    EXPECT_EQ(13000, controlFile.windowGroups()[1].shadeControl()->signalSettings()[0]);
    EXPECT_EQ(10000, controlFile.windowGroups()[1].shadeControl()->signalSettings()[1]);
    EXPECT_EQ(8000, controlFile.windowGroups()[1].shadeControl()->signalSettings()[2]);
    EXPECT_EQ("wg2set1.rad", controlFile.windowGroups()[1].shadeSettingGeometry()[0]);
    EXPECT_EQ("wg2set2.rad", controlFile.windowGroups()[1].shadeSettingGeometry()[1]);
    EXPECT_EQ("l_window", controlFile.windowGroups()[1].bsdfSettingLayers()[0][0]);
    EXPECT_EQ("l_window", controlFile.windowGroups()[1].bsdfSettingLayers()[1][0]);

    EXPECT_EQ("in", controlFile.importUnits());
    EXPECT_EQ("lux", controlFile.illumUnits());
    EXPECT_EQ("ft", controlFile.displayUnits());
    EXPECT_EQ("sched.csv", controlFile.occSchedule());
    EXPECT_EQ(500, controlFile.targetIlluminance());
    EXPECT_EQ(2, controlFile.controlZones().size());

    //Control Zone 1
    EXPECT_EQ("zone1", controlFile.controlZones()[0].name());
    EXPECT_EQ("dim_to_min", controlFile.controlZones()[0].optimumMethod());
    EXPECT_EQ("sensitivity_file", controlFile.controlZones()[0].sensorType());
    EXPECT_EQ("cos.sen", controlFile.controlZones()[0].sensorFile());
    EXPECT_EQ(14, controlFile.controlZones()[0].sensorLocation()[0]);
    EXPECT_EQ(8, controlFile.controlZones()[0].sensorLocation()[1]);
    EXPECT_EQ(10, controlFile.controlZones()[0].sensorLocation()[2]);
    EXPECT_EQ(0, controlFile.controlZones()[0].sensorLocation()[3]);
    EXPECT_EQ(0, controlFile.controlZones()[0].sensorLocation()[4]);
    EXPECT_EQ(-1, controlFile.controlZones()[0].sensorLocation()[5]);
    EXPECT_EQ(0, controlFile.controlZones()[0].sensorLocation()[6]);
    EXPECT_EQ("auto", controlFile.controlZones()[0].cpMethod());
    EXPECT_EQ(3, controlFile.controlZones()[0].numCPs());
    EXPECT_EQ(0.1, controlFile.controlZones()[0].targetPercentage());
    EXPECT_EQ("exclude.pts", controlFile.controlZones()[0].excludedPoints());
    EXPECT_EQ("open_dimming", controlFile.controlZones()[0].algorithm());
    EXPECT_EQ(3000, controlFile.controlZones()[0].maximumBFSignal());
    EXPECT_EQ(100, controlFile.controlZones()[0].minimumBFSignal());
    EXPECT_EQ(100, controlFile.controlZones()[0].offSignal());
    EXPECT_EQ("pendant.ies", controlFile.controlZones()[0].iesFile());
    EXPECT_EQ(0.9, controlFile.controlZones()[0].llf());
    EXPECT_EQ(2700, controlFile.controlZones()[0].lampLumens());
    EXPECT_EQ("linear_dimming", controlFile.controlZones()[0].ballastType());
    EXPECT_EQ(1.0, controlFile.controlZones()[0].bfMax());
    EXPECT_EQ(0.01, controlFile.controlZones()[0].bfMin());
    EXPECT_EQ(28, controlFile.controlZones()[0].wattsMax());
    EXPECT_EQ(10, controlFile.controlZones()[0].wattsMin());
    EXPECT_EQ(5, controlFile.controlZones()[0].luminaireLayout().size());
    //Luminaire1
    EXPECT_EQ(4, controlFile.controlZones()[0].luminaireLayout()[0][0]);
    EXPECT_EQ(4, controlFile.controlZones()[0].luminaireLayout()[0][1]);
    EXPECT_EQ(10, controlFile.controlZones()[0].luminaireLayout()[0][2]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[0][3]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[0][4]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[0][5]);
    //Luminaire2
    EXPECT_EQ(10, controlFile.controlZones()[0].luminaireLayout()[1][0]);
    EXPECT_EQ(4, controlFile.controlZones()[0].luminaireLayout()[1][1]);
    EXPECT_EQ(10, controlFile.controlZones()[0].luminaireLayout()[1][2]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[1][3]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[1][4]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[1][5]);
    //Luminaire3
    EXPECT_EQ(16, controlFile.controlZones()[0].luminaireLayout()[2][0]);
    EXPECT_EQ(4, controlFile.controlZones()[0].luminaireLayout()[2][1]);
    EXPECT_EQ(10, controlFile.controlZones()[0].luminaireLayout()[2][2]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[2][3]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[2][4]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[2][5]);
    //Luminaire4
    EXPECT_EQ(22, controlFile.controlZones()[0].luminaireLayout()[3][0]);
    EXPECT_EQ(4, controlFile.controlZones()[0].luminaireLayout()[3][1]);
    EXPECT_EQ(10, controlFile.controlZones()[0].luminaireLayout()[3][2]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[3][3]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[3][4]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[3][5]);
    //Luminaire5
    EXPECT_EQ(28, controlFile.controlZones()[0].luminaireLayout()[4][0]);
    EXPECT_EQ(4, controlFile.controlZones()[0].luminaireLayout()[4][1]);
    EXPECT_EQ(10, controlFile.controlZones()[0].luminaireLayout()[4][2]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[4][3]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[4][4]);
    EXPECT_EQ(0, controlFile.controlZones()[0].luminaireLayout()[4][5]);

    //Control Zone 2
    EXPECT_EQ("zone2", controlFile.controlZones()[1].name());
    EXPECT_EQ("on", controlFile.controlZones()[1].optimumMethod());
    EXPECT_EQ("downlight.ies", controlFile.controlZones()[1].iesFile());
    EXPECT_EQ(0.85, controlFile.controlZones()[1].llf());
    EXPECT_EQ(2900, controlFile.controlZones()[1].lampLumens());
    EXPECT_EQ("non_dimming", controlFile.controlZones()[1].ballastType());
    EXPECT_EQ(1.0, controlFile.controlZones()[1].ballastFactor());
    EXPECT_EQ(32, controlFile.controlZones()[1].watts());
    EXPECT_EQ(5, controlFile.controlZones()[1].luminaireLayout().size());
    //Luminaire1
    EXPECT_EQ(4, controlFile.controlZones()[1].luminaireLayout()[0][0]);
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[0][1]);
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[0][2]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[0][3]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[0][4]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[0][5]);
    //Luminaire2
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[1][0]);
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[1][1]);
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[1][2]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[1][3]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[1][4]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[1][5]);
    //Luminaire3
    EXPECT_EQ(16, controlFile.controlZones()[1].luminaireLayout()[2][0]);
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[2][1]);
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[2][2]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[2][3]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[2][4]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[2][5]);
    //Luminaire4
    EXPECT_EQ(22, controlFile.controlZones()[1].luminaireLayout()[3][0]);
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[3][1]);
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[3][2]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[3][3]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[3][4]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[3][5]);
    //Luminaire5
    EXPECT_EQ(28, controlFile.controlZones()[1].luminaireLayout()[4][0]);
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[4][1]);
    EXPECT_EQ(10, controlFile.controlZones()[1].luminaireLayout()[4][2]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[4][3]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[4][4]);
    EXPECT_EQ(0, controlFile.controlZones()[1].luminaireLayout()[4][5]);

    EXPECT_EQ(3, controlFile.skyDivisions());
    EXPECT_EQ(4, controlFile.sunDivisions());
    EXPECT_EQ(4, controlFile.ab());
    EXPECT_EQ(300, controlFile.ad());
    EXPECT_EQ(20, controlFile.as());
    EXPECT_EQ(150, controlFile.ar());
    EXPECT_EQ(0.1, controlFile.aa());
    EXPECT_EQ(6, controlFile.lr());
    EXPECT_EQ(0.1500, controlFile.st());
    EXPECT_EQ(1.0, controlFile.sj());
    EXPECT_EQ(0.004, controlFile.lw());
    EXPECT_EQ(0.000, controlFile.dj());
    EXPECT_EQ(0.200, controlFile.ds());
    EXPECT_EQ(2, controlFile.dr());
    EXPECT_EQ(512, controlFile.dp());
    EXPECT_EQ(true, controlFile.daylightSavingsTime());
    EXPECT_EQ(true, controlFile.runsDA());
    EXPECT_EQ(300, controlFile.sDAIllum());
    EXPECT_EQ(0.5, controlFile.sDAFrac());
    EXPECT_EQ(8, controlFile.sDAStart());
    EXPECT_EQ(17, controlFile.sDAEnd());
    EXPECT_EQ(true, controlFile.runOccsDA());
    EXPECT_EQ(300, controlFile.occsDAIllum());
    EXPECT_EQ(0.5, controlFile.occsDAFrac());
    EXPECT_EQ(true, controlFile.runDA());
    EXPECT_EQ(300, controlFile.DAIllum());
    EXPECT_EQ(true, controlFile.runcDA());
    EXPECT_EQ(300, controlFile.cDAIllum());
    EXPECT_EQ(true, controlFile.runDF());
    EXPECT_EQ(true, controlFile.runUDI());
    EXPECT_EQ(100, controlFile.UDIMin());
    EXPECT_EQ(250, controlFile.UDIMax());
}

