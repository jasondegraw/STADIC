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

#include "buildingcontrol.h"
#include "spacecontrol.h"
#include "gtest/gtest.h"
#include <iostream>

TEST(ControlTests, JsonCppReadJson)
{
    stadic::BuildingControl controlFile;
    ASSERT_TRUE(controlFile.parseJson("control.json"));
    //General Arguments
    EXPECT_EQ("in", controlFile.importUnits().get());
    EXPECT_EQ("lux", controlFile.illumUnits().get());
    EXPECT_EQ("ft", controlFile.displayUnits().get());
    EXPECT_TRUE(controlFile.daylightSavingsTime().get());
    EXPECT_EQ("USA_PA_Lancaster.AP.725116_TMY3.epw", controlFile.weaDataFile().get());
    EXPECT_EQ(1,controlFile.firstDay().get());
    EXPECT_EQ(0,controlFile.buildingRotation().get());
    EXPECT_EQ(300, controlFile.targetIlluminance().get());
    EXPECT_EQ(3, controlFile.skyDivisions().get());
    EXPECT_EQ(4, controlFile.sunDivisions().get());
    //DMX Parameters
    EXPECT_EQ("2", controlFile.getRadParam("dmx", "ab").get());
    EXPECT_EQ("1024", controlFile.getRadParam("dmx", "ad").get());
    EXPECT_EQ("256", controlFile.getRadParam("dmx", "as").get());
    EXPECT_EQ("150", controlFile.getRadParam("dmx", "ar").get());
    EXPECT_EQ("0.1", controlFile.getRadParam("dmx", "aa").get());
    EXPECT_EQ("6", controlFile.getRadParam("dmx", "lr").get());
    EXPECT_EQ("0.15", controlFile.getRadParam("dmx", "st").get());
    EXPECT_EQ("1", controlFile.getRadParam("dmx", "sj").get());
    EXPECT_EQ("0.004", controlFile.getRadParam("dmx", "lw").get());
    EXPECT_EQ("0", controlFile.getRadParam("dmx", "dj").get());
    EXPECT_EQ("0.2", controlFile.getRadParam("dmx", "ds").get());
    EXPECT_EQ("2", controlFile.getRadParam("dmx", "dr").get());
    EXPECT_EQ("1", controlFile.getRadParam("dmx", "dp").get());
    EXPECT_EQ("1", controlFile.getRadParam("dmx", "dc").get());
    EXPECT_EQ("0.5", controlFile.getRadParam("dmx", "dt").get());
    //VMX Parameters
    EXPECT_EQ("12", controlFile.getRadParam("vmx", "ab").get());
    EXPECT_EQ("50000", controlFile.getRadParam("vmx", "ad").get());
    EXPECT_EQ("256", controlFile.getRadParam("vmx", "as").get());
    EXPECT_EQ("150", controlFile.getRadParam("vmx", "ar").get());
    EXPECT_EQ("0.1", controlFile.getRadParam("vmx", "aa").get());
    EXPECT_EQ("6", controlFile.getRadParam("vmx", "lr").get());
    EXPECT_EQ("0.15", controlFile.getRadParam("vmx", "st").get());
    EXPECT_EQ("1", controlFile.getRadParam("vmx", "sj").get());
    EXPECT_EQ("0.000152", controlFile.getRadParam("vmx", "lw").get());
    EXPECT_EQ("1", controlFile.getRadParam("vmx", "dj").get());
    EXPECT_EQ("0.2", controlFile.getRadParam("vmx", "ds").get());
    EXPECT_EQ("2", controlFile.getRadParam("vmx", "dr").get());
    EXPECT_EQ("1", controlFile.getRadParam("vmx", "dp").get());
    EXPECT_EQ("1", controlFile.getRadParam("vmx", "dc").get());
    EXPECT_EQ("0.5", controlFile.getRadParam("vmx", "dt").get());
    //Default Parameters
    EXPECT_EQ("5", controlFile.getRadParam("default", "ab").get());
    EXPECT_EQ("10000", controlFile.getRadParam("default", "ad").get());
    EXPECT_EQ("256", controlFile.getRadParam("default", "as").get());
    EXPECT_EQ("150", controlFile.getRadParam("default", "ar").get());
    EXPECT_EQ("0.1", controlFile.getRadParam("default", "aa").get());
    EXPECT_EQ("6", controlFile.getRadParam("default", "lr").get());
    EXPECT_EQ("0.15", controlFile.getRadParam("default", "st").get());
    EXPECT_EQ("1", controlFile.getRadParam("default", "sj").get());
    EXPECT_EQ("0.004", controlFile.getRadParam("default", "lw").get());
    EXPECT_EQ("1", controlFile.getRadParam("default", "dj").get());
    EXPECT_EQ("0.2", controlFile.getRadParam("default", "ds").get());
    EXPECT_EQ("2", controlFile.getRadParam("default", "dr").get());
    EXPECT_EQ("1", controlFile.getRadParam("default", "dp").get());
    EXPECT_EQ("1", controlFile.getRadParam("default", "dc").get());
    EXPECT_EQ("0.5", controlFile.getRadParam("default", "dt").get());
    EXPECT_EQ(1, controlFile.spaces().size());
    //Spaces
    stadic::Control *model=controlFile.spaces()[0].get();
    EXPECT_EQ("prj1", model->spaceName());
    EXPECT_EQ("c:/example/", model->spaceDirectory());
    EXPECT_EQ("rad/", model->geoDirectory());
    EXPECT_EQ("ies/", model->iesDirectory());
    EXPECT_EQ("res/", model->resultsDirectory());
    EXPECT_EQ("data/", model->inputDirectory());
    EXPECT_EQ("sched.csv", model->occSchedule());
    EXPECT_EQ("sched.csv", model->lightSchedule());
    EXPECT_EQ(0.2, model->groundReflect());


  EXPECT_EQ("USA_PA_Lancaster.AP.725116_TMY3.epw", model->weaFile());
  EXPECT_EQ(1, model->firstDay().get());
  EXPECT_EQ("main_mat.rad", model->matFile());
  EXPECT_EQ("main_geo.rad", model->geoFile());
  EXPECT_EQ(0,model->buildingRotation());
  EXPECT_EQ(2, model->ptsFile().size());
  EXPECT_EQ("calc_grids.pts", model->ptsFile()[0]);
  EXPECT_EQ("analysis_grid.pts", model->ptsFile()[1]);
  EXPECT_EQ(2,model->windowGroups().size());

    //Window Group 1
    EXPECT_EQ("WG1", model->windowGroups()[0].name());
    EXPECT_EQ(2, model->windowGroups()[0].shadeSettingGeometry().size());
    EXPECT_EQ("wg1base.rad", model->windowGroups()[0].baseGeometry());
    EXPECT_EQ("l_glazing", model->windowGroups()[0].glazingLayers()[0]);
    EXPECT_EQ("l_glazing2", model->windowGroups()[0].glazingLayers()[1]);
    EXPECT_EQ("automated_profile_angle", model->windowGroups()[0].shadeControl()->controlMethod());
    EXPECT_EQ(30, model->windowGroups()[0].shadeControl()->elevationAzimuth());
    EXPECT_EQ(30, model->windowGroups()[0].shadeControl()->angleSettings()[0]);
    EXPECT_EQ(60, model->windowGroups()[0].shadeControl()->angleSettings()[1]);
    EXPECT_EQ("wg1set1.rad", model->windowGroups()[0].shadeSettingGeometry()[0]);
    EXPECT_EQ("wg1set2.rad", model->windowGroups()[0].shadeSettingGeometry()[1]);

    //Window Group 2
    EXPECT_EQ("WG2", model->windowGroups()[1].name());
    EXPECT_EQ(2, model->windowGroups()[1].shadeSettingGeometry().size());
    EXPECT_EQ(true, model->windowGroups()[1].isBSDF());
    EXPECT_EQ("wg2base.rad", model->windowGroups()[1].baseGeometry());
    EXPECT_EQ("l_window", model->windowGroups()[1].glazingLayers()[0]);
    EXPECT_EQ("l_window", model->windowGroups()[1].bsdfBaseLayers()[0]);

    EXPECT_EQ("automated_signal", model->windowGroups()[1].shadeControl()->controlMethod());
    EXPECT_EQ("sensitivity_file", model->windowGroups()[1].shadeControl()->sensorType());
    EXPECT_EQ("cos.sen", model->windowGroups()[1].shadeControl()->sensorFile());
    EXPECT_EQ(4, model->windowGroups()[1].shadeControl()->location()[0]);
    EXPECT_EQ(0, model->windowGroups()[1].shadeControl()->location()[1]);
    EXPECT_EQ(5, model->windowGroups()[1].shadeControl()->location()[2]);
    EXPECT_EQ(0, model->windowGroups()[1].shadeControl()->location()[3]);
    EXPECT_EQ(-1, model->windowGroups()[1].shadeControl()->location()[4]);
    EXPECT_EQ(0, model->windowGroups()[1].shadeControl()->location()[5]);
    EXPECT_EQ(0, model->windowGroups()[1].shadeControl()->location()[6]);
    ASSERT_EQ(2, model->windowGroups()[1].shadeControl()->signalSettings().size());
    EXPECT_EQ(13000, model->windowGroups()[1].shadeControl()->signalSettings()[0]);
    EXPECT_EQ(10000, model->windowGroups()[1].shadeControl()->signalSettings()[1]);
    EXPECT_EQ("wg2set1.rad", model->windowGroups()[1].shadeSettingGeometry()[0]);
    EXPECT_EQ("wg2set2.rad", model->windowGroups()[1].shadeSettingGeometry()[1]);
    ASSERT_EQ(2, model->windowGroups()[1].bsdfSettingLayers().size());
    ASSERT_EQ(1, model->windowGroups()[1].bsdfSettingLayers()[0].size());
    EXPECT_EQ("l_window", model->windowGroups()[1].bsdfSettingLayers()[0][0]);
    ASSERT_EQ(1, model->windowGroups()[1].bsdfSettingLayers()[1].size());
    EXPECT_EQ("l_window", model->windowGroups()[1].bsdfSettingLayers()[1][0]);

    EXPECT_EQ("in", model->importUnits());
    EXPECT_EQ("lux", model->illumUnits());
    EXPECT_EQ("ft", model->displayUnits());
    EXPECT_EQ("sched.csv", model->occSchedule());
    EXPECT_EQ(300, model->targetIlluminance());
    EXPECT_EQ(2, model->controlZones().size());

    //Control Zone 1
    EXPECT_EQ("zone1", model->controlZones()[0].name());
    EXPECT_EQ("dim_to_min", model->controlZones()[0].optimumMethod());
    EXPECT_EQ("sensitivity_file", model->controlZones()[0].sensorType());
    EXPECT_EQ("cos.sen", model->controlZones()[0].sensorFile());
    EXPECT_EQ(14, model->controlZones()[0].sensorLocation()[0]);
    EXPECT_EQ(8, model->controlZones()[0].sensorLocation()[1]);
    EXPECT_EQ(10, model->controlZones()[0].sensorLocation()[2]);
    EXPECT_EQ(0, model->controlZones()[0].sensorLocation()[3]);
    EXPECT_EQ(0, model->controlZones()[0].sensorLocation()[4]);
    EXPECT_EQ(-1, model->controlZones()[0].sensorLocation()[5]);
    EXPECT_EQ(0, model->controlZones()[0].sensorLocation()[6]);
    EXPECT_EQ("auto", model->controlZones()[0].cpMethod());
    EXPECT_EQ(3, model->controlZones()[0].numCPs());
    EXPECT_EQ(0.1, model->controlZones()[0].targetPercentage());
    EXPECT_EQ("exclude.pts", model->controlZones()[0].excludedPoints());
    EXPECT_EQ("open_dimming", model->controlZones()[0].algorithm());
    EXPECT_EQ(3000, model->controlZones()[0].maximumBFSignal());
    EXPECT_EQ(100, model->controlZones()[0].minimumBFSignal());
    EXPECT_EQ(100, model->controlZones()[0].offSignal());
    EXPECT_EQ("pendant.ies", model->controlZones()[0].iesFile());
    EXPECT_EQ(0.9, model->controlZones()[0].llf());
    EXPECT_EQ(2700, model->controlZones()[0].lampLumens());
    EXPECT_EQ("linear_dimming", model->controlZones()[0].ballastType());
    EXPECT_EQ(1.0, model->controlZones()[0].bfMax());
    EXPECT_EQ(0.01, model->controlZones()[0].bfMin());
    EXPECT_EQ(28, model->controlZones()[0].wattsMax());
    EXPECT_EQ(10, model->controlZones()[0].wattsMin());
    EXPECT_EQ(5, model->controlZones()[0].luminaireLayout().size());
    //Luminaire1
    EXPECT_EQ(4, model->controlZones()[0].luminaireLayout()[0][0]);
    EXPECT_EQ(4, model->controlZones()[0].luminaireLayout()[0][1]);
    EXPECT_EQ(10, model->controlZones()[0].luminaireLayout()[0][2]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[0][3]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[0][4]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[0][5]);
    //Luminaire2
    EXPECT_EQ(10, model->controlZones()[0].luminaireLayout()[1][0]);
    EXPECT_EQ(4, model->controlZones()[0].luminaireLayout()[1][1]);
    EXPECT_EQ(10, model->controlZones()[0].luminaireLayout()[1][2]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[1][3]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[1][4]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[1][5]);
    //Luminaire3
    EXPECT_EQ(16, model->controlZones()[0].luminaireLayout()[2][0]);
    EXPECT_EQ(4, model->controlZones()[0].luminaireLayout()[2][1]);
    EXPECT_EQ(10, model->controlZones()[0].luminaireLayout()[2][2]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[2][3]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[2][4]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[2][5]);
    //Luminaire4
    EXPECT_EQ(22, model->controlZones()[0].luminaireLayout()[3][0]);
    EXPECT_EQ(4, model->controlZones()[0].luminaireLayout()[3][1]);
    EXPECT_EQ(10, model->controlZones()[0].luminaireLayout()[3][2]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[3][3]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[3][4]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[3][5]);
    //Luminaire5
    EXPECT_EQ(28, model->controlZones()[0].luminaireLayout()[4][0]);
    EXPECT_EQ(4, model->controlZones()[0].luminaireLayout()[4][1]);
    EXPECT_EQ(10, model->controlZones()[0].luminaireLayout()[4][2]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[4][3]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[4][4]);
    EXPECT_EQ(0, model->controlZones()[0].luminaireLayout()[4][5]);

    //Control Zone 2
    EXPECT_EQ("zone2", model->controlZones()[1].name());
    EXPECT_EQ("on", model->controlZones()[1].optimumMethod());
    EXPECT_EQ("downlight.ies", model->controlZones()[1].iesFile());
    EXPECT_EQ(0.85, model->controlZones()[1].llf());
    EXPECT_EQ(2900, model->controlZones()[1].lampLumens());
    EXPECT_EQ("non_dimming", model->controlZones()[1].ballastType());
    EXPECT_EQ(1.0, model->controlZones()[1].ballastFactor());
    EXPECT_EQ(32, model->controlZones()[1].watts());
    EXPECT_EQ(5, model->controlZones()[1].luminaireLayout().size());
    //Luminaire1
    EXPECT_EQ(4, model->controlZones()[1].luminaireLayout()[0][0]);
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[0][1]);
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[0][2]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[0][3]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[0][4]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[0][5]);
    //Luminaire2
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[1][0]);
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[1][1]);
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[1][2]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[1][3]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[1][4]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[1][5]);
    //Luminaire3
    EXPECT_EQ(16, model->controlZones()[1].luminaireLayout()[2][0]);
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[2][1]);
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[2][2]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[2][3]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[2][4]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[2][5]);
    //Luminaire4
    EXPECT_EQ(22, model->controlZones()[1].luminaireLayout()[3][0]);
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[3][1]);
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[3][2]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[3][3]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[3][4]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[3][5]);
    //Luminaire5
    EXPECT_EQ(28, model->controlZones()[1].luminaireLayout()[4][0]);
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[4][1]);
    EXPECT_EQ(10, model->controlZones()[1].luminaireLayout()[4][2]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[4][3]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[4][4]);
    EXPECT_EQ(0, model->controlZones()[1].luminaireLayout()[4][5]);

    EXPECT_EQ(3, model->skyDivisions());
    EXPECT_EQ(4, model->sunDivisions());
    EXPECT_EQ(true, model->daylightSavingsTime());
    EXPECT_EQ(true, model->runsDA());
    EXPECT_EQ(300, model->sDAIllum());
    EXPECT_EQ(0.5, model->sDAFrac());
    EXPECT_EQ(8, model->sDAStart());
    EXPECT_EQ(17, model->sDAEnd());
    EXPECT_EQ(true, model->runOccsDA());
    EXPECT_EQ(300, model->occsDAIllum());
    EXPECT_EQ(0.5, model->occsDAFrac());
    EXPECT_EQ(true, model->runDA());
    EXPECT_EQ(300, model->DAIllum());
    EXPECT_EQ(true, model->runcDA());
    EXPECT_EQ(300, model->cDAIllum());
    EXPECT_EQ(true, model->runDF());
    EXPECT_EQ(true, model->runUDI());
    EXPECT_EQ(100, model->UDIMin());
    EXPECT_EQ(250, model->UDIMax());

}

