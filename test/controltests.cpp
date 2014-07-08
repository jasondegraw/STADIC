#include "../lib/stadiccontrol.h"
#include <iostream>

int main(int argc, char *argv[])
{
    StadicControl controlFile;
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
    std::cout<<"project_name "<<controlFile.projectName().toStdString()<<std::endl;
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
}
