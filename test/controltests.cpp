#include "stadiccontrol.h"
#include "gtest/gtest.h"
#include <iostream>

TEST(ControlTests, ReadJson)
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
    EXPECT_EQ(300, controlFile.controlZones()[0].offSignal());
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





  /*

    if (argc<2){
        if (!controlFile.parseJson(":/resources/TestSControl.json")){
            std::cerr<<"The program has encountered an error reading the STADIC control file.\n\tThe program will now close.\n";
            return false;
        }
    }else{
        if (!controlFile.parseJson(QString(argv[1]))){
            std::cerr<<"The program has encountered an error reading the STADIC control file.\n\tThe program will now close.\n";
            return false;
        }
    }
    std::cout<<"project_name "<<.toStdString()<<std::endl;
    std::cout<<"project_folder "<<controlFile.projectFolder().toStdString()<<std::endl;
    std::cout<<"tmp_folder "<<controlFile.tmpFolder().toStdString()<<std::endl;
    std::cout<<"geometry_folder "<<controlFile.geoFolder().toStdString()<<std::endl;
    std::cout<<"ies_folder "<<controlFile.iesFolder().toStdString()<<std::endl;
    std::cout<<"results_folder "<<controlFile.resultsFolder().toStdString()<<std::endl;
    std::cout<<"data_folder "<<controlFile.dataFolder().toStdString()<<std::endl<<std::endl;
    std::cout<<"ground_reflectance "<<controlFile.groundReflect()<<std::endl;
    std::cout<<"wea_data_file "<<controlFile.weaDataFile().toStdString()<<std::endl;
    std::cout<<"first_day "<<controlFile.firstDay()<<std::endl<<std::endl;
    std::cout<<"material_file "<<controlFile.matFile().toStdString()<<std::endl;
    std::cout<<"geometry_file "<<controlFile.geoFile().toStdString()<<std::endl;
    std::cout<<"building_rotation "<<controlFile.buildingRotation()<<std::endl;
    std::cout<<"analysis_points "<<controlFile.ptsFile().toStdString()<<std::endl;
    std::cout<<"number of windows groups "<<controlFile.windowGroups().size()<<std::endl;
    for (unsigned int i=0;i<controlFile.windowGroups().size();i++){
        std::cout<<"\twindow group "<<i+1<<" name is "<<controlFile.windowGroups()[i]->objectName().toStdString()<<std::endl;
        if (controlFile.windowGroups()[i]->isBSDF()){
            std::cout<<"\t\tContains BSDFs"<<std::endl;
            if (controlFile.windowGroups()[i]->bsdfBaseLayers().size()>0){
                std::cout<<"\t\tbsdf_base_layers";
                for (unsigned int j=0;j<controlFile.windowGroups()[i]->bsdfBaseLayers().size();j++){
                    std::cout<<" "<<controlFile.windowGroups()[i]->bsdfBaseLayers()[j].toStdString();
                }
                std::cout<<std::endl;
            }

        }
        std::cout<<"\t\tbase_geometry file is "<<controlFile.windowGroups()[i]->baseGeometry().toStdString()<<std::endl;
        if (controlFile.windowGroups()[i]->glazingLayers().size()>0){
            std::cout<<"\t\tglazing_layers";
            for (unsigned int j=0;j<controlFile.windowGroups()[i]->glazingLayers().size();j++){
                std::cout<<" "<<controlFile.windowGroups()[i]->glazingLayers()[j].toStdString();
            }
            std::cout<<std::endl;
        }
        std::cout<<"\t\tshade_control"<<std::endl;
        std::cout<<"\t\t\tmethod "<<controlFile.windowGroups()[i]->shadeControl()->controlMethod().toStdString()<<std::endl;
        if (controlFile.windowGroups()[i]->shadeControl()->controlMethod()=="automated_profile_angle"){
            std::cout<<"\t\t\televation_azimuth "<<controlFile.windowGroups()[i]->shadeControl()->elevationAzimuth()<<std::endl;
            std::cout<<"\t\t\tangle_settings";
            for (int j=0;j<controlFile.windowGroups()[i]->shadeControl()->angleSettings().size();j++){
                std::cout<<" "<<controlFile.windowGroups()[i]->shadeControl()->angleSettings()[j];
            }
            std::cout<<std::endl;
        }else if(controlFile.windowGroups()[i]->shadeControl()->controlMethod()=="automated_signal"){
            std::cout<<"\t\t\tlocation";
            for (int j=0;j<controlFile.windowGroups()[i]->shadeControl()->location().size();j++){
                std::cout<<" "<<controlFile.windowGroups()[i]->shadeControl()->location()[j];
            }
            std::cout<<std::endl;
            std::cout<<"\t\t\tsignal_settings";
            for (int j=0;j<controlFile.windowGroups()[i]->shadeControl()->signalSettings().size();j++){
                std::cout<<" "<<controlFile.windowGroups()[i]->shadeControl()->signalSettings()[j];
            }
            std::cout<<std::endl;
        }else if(controlFile.windowGroups()[i]->shadeControl()->controlMethod()=="automated_profile_angle_signal"){

        }

        if (controlFile.windowGroups()[i]->shadeSettingGeometry().size()>0){
            std::cout<<"\t\tshade_settings";
            for (unsigned int j=0;j<controlFile.windowGroups()[i]->shadeSettingGeometry().size();j++){
                std::cout<<"\n\t\t\tgeometry "<<controlFile.windowGroups()[i]->shadeSettingGeometry()[j].toStdString();
            }
            std::cout<<std::endl;
        }
        if (controlFile.windowGroups()[i]->bsdfSettingLayers().size()>0){
            std::cout<<"\t\tbsdf_setting_layers";
            for (unsigned int j=0;j<controlFile.windowGroups()[i]->bsdfSettingLayers().size();j++){
                std::cout<<"\n\t\t\t";
                for (unsigned int k=0;k<controlFile.windowGroups()[i]->bsdfSettingLayers()[j].size();k++){
                    std::cout<<controlFile.windowGroups()[i]->bsdfSettingLayers()[j][k].toStdString()<<" ";
                }
            }
            std::cout<<std::endl;
        }

    }
    std::cout<<"import_units "<<controlFile.importUnits().toStdString()<<std::endl;
    std::cout<<"illum_units "<<controlFile.illumUnits().toStdString()<<std::endl;
    std::cout<<"display_units "<<controlFile.displayUnits().toStdString()<<std::endl;
    std::cout<<"occupancy_schedule "<<controlFile.occSchedule().toStdString()<<std::endl;
    std::cout<<"target_illuminance "<<controlFile.targetIlluminance()<<std::endl;
    std::cout<<"daylight_savings_time "<<controlFile.daylightSavingsTime()<<std::endl;
    std::cout<<"sun_divisions "<<controlFile.sunDivisions()<<std::endl;
    std::cout<<"sky_divisions "<<controlFile.skyDivisions()<<std::endl;
    std::cout<<"radiance_parameters"<<std::endl;
    std::cout<<"\tab "<<controlFile.ab()<<std::endl;
    std::cout<<"\tad "<<controlFile.ad()<<std::endl;
    std::cout<<"\tas "<<controlFile.as()<<std::endl;
    std::cout<<"\tar "<<controlFile.ar()<<std::endl;
    std::cout<<"\taa "<<controlFile.aa()<<std::endl;
    std::cout<<"\tlr "<<controlFile.lr()<<std::endl;
    std::cout<<"\tst "<<controlFile.st()<<std::endl;
    std::cout<<"\tsj "<<controlFile.sj()<<std::endl;
    std::cout<<"\tlw "<<controlFile.lw()<<std::endl;
    std::cout<<"\tdj "<<controlFile.dj()<<std::endl;
    std::cout<<"\tds "<<controlFile.ds()<<std::endl;
    std::cout<<"\tdr "<<controlFile.dr()<<std::endl;
    std::cout<<"\tdp "<<controlFile.dp()<<std::endl;

    return EXIT_SUCCESS;
    */
}
