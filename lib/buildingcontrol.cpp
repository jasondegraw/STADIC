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

#include "buildingcontrol.h"
#include "windowgroup.h"
#include <iostream>
#include <fstream>
#include <string>
#include "functions.h"

namespace stadic {

BuildingControl::BuildingControl()
{
}

//Setters
//******************
//Folder Information
//******************
void BuildingControl::setProjectName(std::string name){
    m_ProjectName=name;
}
void BuildingControl::setProjectFolder(std::string folder){
    m_ProjectFolder=folder;
}
void BuildingControl::setTmpFolder(std::string folder){
    m_TmpFolder=folder;
}
void BuildingControl::setGeoFolder(std::string folder){
    m_GeoFolder=folder;
}
void BuildingControl::setIESFolder(std::string folder){
    m_IESFolder=folder;
}
void BuildingControl::setResultsFolder(std::string folder){
    m_ResultsFolder=folder;
}
void BuildingControl::setDataFolder(std::string folder){
    m_DataFolder=folder;
}

//******************
//Site Information
//******************
bool BuildingControl::setGroundReflect(double value){
    if (value>1 || value<0){
        STADIC_WARNING("The ground reflectance must be between 0 and  1.\n\tA default value of 0.2 will be applied.");
        //std::cerr<<"WARNING: The ground reflectance must be between 0 and  1.\n\tA default value of 0.2 will be applied."<<std::endl;
        m_GroundReflect=0.2;
    }else{
        m_GroundReflect=value;
    }
    return true;
}
void BuildingControl::setWeaDataFile(std::string file){
    m_WeaDataFile=file;
}
bool BuildingControl::setFirstDay(int value){
    if (value>7 || value<1){
        STADIC_WARNING("The first day must be an integer between 1 and 7.\n\tA default value of 1 will be applied.");
        m_FirstDay=1;
    }else{
        m_FirstDay=value;
    }
    return true;
}

//******************
//Geometry Information
//******************
void BuildingControl::setMatFile(std::string file){
    m_MatFile=file;
}
void BuildingControl::setGeoFile(std::string file){
    m_GeoFile=file;
}
bool BuildingControl::setBuildingRotation(double value){
    if (value>360 || value<-360){
        STADIC_ERROR("The building rotation must be between -360 and 360.");
        return false;
    }else{
        m_BuildingRotation=value;
    }
    return true;
}
void BuildingControl::setPTSFile(std::string file){
    m_PTSFile=file;
}
bool BuildingControl::setImportUnits(std::string units){
    if (units=="ft" || units=="in" || units=="mm" || units=="m"){
        m_ImportUnits=units;
    }else{
        STADIC_ERROR("The import units must be one of the following:\n\t\"ft\", \"in\", \"mm\", \"m\"");
        return false;
    }
    return true;
}
void BuildingControl::setIllumUnits(std::string units){
    if (units=="lux" || units=="fc"){
        m_IllumUnits=units;
    }else{
        STADIC_WARNING("The illuminance units must be either \"lux\" or \"fc\".  The default illuminance units will be set to lux.");
        m_IllumUnits="lux";
    }
}
bool BuildingControl::setDisplayUnits(std::string units){
    if (units=="ft" || units=="in" || units=="mm" || units=="m"){
        m_DisplayUnits=units;
    }else{
        STADIC_ERROR("The display units must be one of the following:\n\t\"ft\", \"in\", \"mm\", \"m\"");
        return false;
    }
    return true;
}
void BuildingControl::setOccSchedule(std::string file){
    m_OccSchedule=file;
}
bool BuildingControl::setTargetIlluminance(double value){
    if (value<0){
        STADIC_ERROR("The target illuminance must be greater than 0.");
        return false;
    }else{
        m_TargetIlluminance=value;
    }
    return true;
}

//******************
//Simulation Settings
//******************
bool BuildingControl::setSunDivisions(int value){
    if (value<1 || value>6){
        STADIC_WARNING("The sun divisions must be between 1 and 6.\n\tA default value of 3 will be applied.");
        m_SunDivisions=3;
    }else{
        m_SunDivisions=value;
    }
    return true;
}

bool BuildingControl::setSkyDivisions(int value)
{
    if (value<1 || value>6){
        STADIC_WARNING("The sky divisions must be between 1 and 6.\n\tA default value of 3 will be applied.");
        m_SkyDivisions=3;
    }else{
        m_SkyDivisions=value;
    }
    return true;
}

void BuildingControl::setDaylightSavingsTime(bool value)
{
    m_DaylightSavingsTime=value;
}

bool BuildingControl::setDefaultRadianceParameters()
{
    std::cerr<<"\t\tab=4"<<std::endl;
    if (!setAB(4)){
        return false;
    }
    std::cerr<<"\t\tad=300"<<std::endl;
    if (!setAD(300)){
        return false;
    }
    std::cerr<<"\t\tas=20"<<std::endl;
    if (!setAS(20)){
        return false;
    }
    std::cerr<<"\t\tar=150"<<std::endl;
    if (!setAR(150)){
        return false;
    }
    std::cerr<<"\t\taa=0.1"<<std::endl;
    if (!setAA(0.1)){
        return false;
    }
    std::cerr<<"\t\tlr=6"<<std::endl;
    if (!setLR(6)){
        return false;
    }
    std::cerr<<"\t\tst=0.15"<<std::endl;
    if (!setST(0.15)){
        return false;
    }
    std::cerr<<"\t\tsj=1.0"<<std::endl;
    if (!setSJ(1.0)){
        return false;
    }
    std::cerr<<"\t\tlw=0.004"<<std::endl;
    if (!setLW(0.004)){
        return false;
    }
    std::cerr<<"\t\tdj=0.0000"<<std::endl;
    if (!setDJ(0.0000)){
        return false;
    }
    std::cerr<<"\t\tds=0.200"<<std::endl;
    if (!setDS(0.200)){
        return false;
    }
    std::cerr<<"\t\tdr=2"<<std::endl;
    if (!setDR(2)){
        return false;
    }
    std::cerr<<"\t\tdp=512"<<std::endl;
    if (!setDP(512)){
        return false;
    }
    return true;
}

bool BuildingControl::setAB(int value){
    m_AB=value;
    return true;
}
bool BuildingControl::setAD(int value){
    m_AD=value;
    return true;
}
bool BuildingControl::setAS(int value){
    m_AS=value;
    return true;
}
bool BuildingControl::setAR(int value){
    m_AR=value;
    return true;
}
bool BuildingControl::setAA(double value){
    m_AA=value;
    return true;
}
bool BuildingControl::setLR(int value){
    m_LR=value;
    return true;
}
bool BuildingControl::setST(double value){
    m_ST=value;
    return true;
}
bool BuildingControl::setSJ(double value){
    m_SJ=value;
    return true;
}
bool BuildingControl::setLW(double value){
    m_LW=value;
    return true;
}
bool BuildingControl::setDJ(double value){
    m_DJ=value;
    return true;
}
bool BuildingControl::setDS(double value){
    m_DS=value;
    return true;
}
bool BuildingControl::setDR(int value){
    m_DR=value;
    return true;
}
bool BuildingControl::setDP(double value){
    m_DP=value;
    return true;
}


//******************
//Metrics
//******************
bool BuildingControl::setDA(bool run, double illum){
    m_DA=run;
    if (illum>0){
        m_DAIllum=illum;
    }else{
        STADIC_ERROR("The DA illuminance must be greater than 0.");
        return false;
    }
    return true;
}
bool BuildingControl::setcDA(bool run, double illum){
    m_cDA=run;
    if (illum>0){
        m_cDAIllum=illum;
    }else{
        STADIC_ERROR("The cDA illuminance must be greater than 0.");
        return false;
    }
    return true;
}
bool BuildingControl::setsDA(bool run, double illum, double DAFrac, double startTime, double endTime){
    m_sDA=run;
    if (illum>0){
        m_sDAIllum=illum;
    }else{
        STADIC_ERROR("The sDA illuminance must be greater than 0.");
        return false;
    }
    if (DAFrac>0 && DAFrac<1){
        m_sDAFrac=DAFrac;
    }else{
        STADIC_ERROR("The sDA fraction must be between 0 and 1.");
        return false;
    }
    if (startTime>0 && startTime<endTime){
        m_sDAStart=startTime;
    }else{
        STADIC_ERROR("The sDA start time has to be greater than 0 and less than the end time.");
        return false;
    }
    if (endTime<24 && endTime>startTime){
        m_sDAEnd=endTime;
    }else{
        STADIC_ERROR("The sDA end time has to be greater than the start time and less than 24.");
        return false;
    }
    return true;
}
bool BuildingControl::setOccsDA(bool run, double illum, double DAFrac){
    m_OccsDA=run;
    if (illum>0){
        m_OccsDAIllum=illum;
    }else{
        STADIC_ERROR("The occupancy schedule based sDA illuminance must be greater than 0.");
        return false;
    }
    if (DAFrac>0 && DAFrac<1){
        m_OccsDAFrac=DAFrac;
    }else{
        STADIC_ERROR("The occupancy schedule based sDA fraction must be between 0 and 1.");
        return false;
    }
    return true;
}
void BuildingControl::setDF(bool run){
    m_DF=run;
}
bool BuildingControl::setUDI(bool run, double minIllum, double maxIllum){
    m_UDI=run;
    if (minIllum>=0 && minIllum<maxIllum){
        m_UDIMin=minIllum;
    }else{
        STADIC_ERROR("The UDI minimum illuminance must be between 0 and the maximum illuminance.");
        return false;
    }
    m_UDIMax=maxIllum;
    return true;
}



//Getters
//******************
//Folder Information
//******************
std::string BuildingControl::projectName(){
    return m_ProjectName;
}
std::string BuildingControl::projectFolder(){
    return m_ProjectFolder;
}
std::string BuildingControl::tmpFolder(){
    return m_TmpFolder;
}
std::string BuildingControl::geoFolder(){
    return m_GeoFolder;
}
std::string BuildingControl::iesFolder(){
    return m_IESFolder;
}
std::string BuildingControl::resultsFolder(){
    return m_ResultsFolder;
}
std::string BuildingControl::dataFolder(){
    return m_DataFolder;
}

//******************
//Site Information
//******************
double BuildingControl::groundReflect(){
    return m_GroundReflect;
}
std::string BuildingControl::weaDataFile(){
    return m_WeaDataFile;
}
int BuildingControl::firstDay(){
    return m_FirstDay;
}

//******************
//Geometry Information
//******************
std::string BuildingControl::matFile(){
    return m_MatFile;
}
std::string BuildingControl::geoFile(){
    return m_GeoFile;
}
double BuildingControl::buildingRotation(){
    return m_BuildingRotation;
}
std::string BuildingControl::ptsFile(){
    return m_PTSFile;
}
std::vector<WindowGroup> BuildingControl::windowGroups(){
    return m_WindowGroups;
}
std::string BuildingControl::importUnits(){
    return m_ImportUnits;
}
std::string BuildingControl::illumUnits(){
    return m_IllumUnits;
}
std::string BuildingControl::displayUnits(){
    return m_DisplayUnits;
}
std::string BuildingControl::occSchedule(){
    return m_OccSchedule;
}
double BuildingControl::targetIlluminance(){
    return m_TargetIlluminance;
}

//******************
//Simulation Settings
//******************
int BuildingControl::sunDivisions(){
    return m_SunDivisions;
}
int BuildingControl::skyDivisions(){
    return m_SkyDivisions;
}

bool BuildingControl::daylightSavingsTime(){
    return m_DaylightSavingsTime;
}
int BuildingControl::ab(){
    return m_AB;
}
int BuildingControl::ad(){
    return m_AD;
}
int BuildingControl::as(){
    return m_AS;
}
int BuildingControl::ar(){
    return m_AR;
}
double BuildingControl::aa(){
    return m_AA;
}
int BuildingControl::lr(){
    return m_LR;
}
double BuildingControl::st(){
    return m_ST;
}
double BuildingControl::sj(){
    return m_SJ;
}
double BuildingControl::lw(){
    return m_LW;
}
double BuildingControl::dj(){
    return m_DJ;
}
double BuildingControl::ds(){
    return m_DS;
}
int BuildingControl::dr(){
    return m_DR;
}
double BuildingControl::dp(){
    return m_DP;
}

//******************
//Lighting BuildingControl
//******************
std::vector<ControlZone> BuildingControl::controlZones(){
    return m_ControlZones;
}

//******************
//Metrics
//******************
bool BuildingControl::runDA(){
    return m_DA;
}
double BuildingControl::DAIllum(){
    return m_DAIllum;
}
bool BuildingControl::runsDA(){
    return m_sDA;
}
double BuildingControl::sDAIllum(){
    return m_sDAIllum;
}
double BuildingControl::sDAFrac(){
    return m_sDAFrac;
}
double BuildingControl::sDAStart(){
    return m_sDAStart;
}
double BuildingControl::sDAEnd(){
    return m_sDAEnd;
}
bool BuildingControl::runOccsDA(){
    return m_OccsDA;
}
double BuildingControl::occsDAIllum(){
    return m_OccsDAIllum;
}
double BuildingControl::occsDAFrac(){
    return m_OccsDAFrac;
}
bool BuildingControl::runcDA(){
    return m_cDA;
}
double BuildingControl::cDAIllum(){
    return m_cDAIllum;
}
bool BuildingControl::runDF(){
    return m_DF;
}
bool BuildingControl::runUDI(){
    return m_UDI;
}
double BuildingControl::UDIMin(){
    return m_UDIMin;
}
double BuildingControl::UDIMax(){
    return m_UDIMax;
}


//******************
//PARSER
//******************

bool BuildingControl::parseJson(const std::string &file)
{
    boost::optional<JsonObject> jsonOpt = readJsonDocument(file);

    if(!jsonOpt){
        STADIC_LOG(Severity::Fatal, "Failed to read json input file \"" + file + "\".");
        return false;
    }

    JsonObject json = jsonOpt.get();

    //get_value_or(/*default*/);
    boost::optional<std::string> sVal;
    boost::optional<double> dVal;
    boost::optional<int> iVal;
    boost::optional<bool> bVal;
    boost::optional<JsonObject> treeVal;

    //******************
    //Folder Information
    //******************
    sVal=getString(json, "project_name", "The key \"project_name\" does not appear in the STADIC BuildingControl File.", "The key \"project_name\" does not contain a string.", Severity::Error);
    if(!sVal){
        return false;
    }else{
        setProjectName(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "project_folder", "The key \"project_folder\" does not appear in the STADIC BuildingControl File.", "The key \"project_folder\" does not contain a string.", Severity::Error);
    if(!sVal){
        return false;
    }else{
        setProjectFolder(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "tmp_folder", "The key \"tmp_folder\" does not appear in the STADIC BuildingControl File.", "The \"tmp_folder\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setTmpFolder(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "geometry_folder", "The key \"geometry_folder\" does not appear in the STADIC BuildingControl File.", "The \"geometry_folder\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setGeoFolder(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "ies_folder", "The key \"ies_folder\" does not appear in the STADIC BuildingControl File.", "The \"ies_folder\" is not a string.", Severity::Warning);
    if (sVal){
        setIESFolder(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "results_folder", "The key \"results_folder\" does not appear in the STADIC BuildingControl File.", "The \"results_folder\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setResultsFolder(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "data_folder", "The key \"data_folder\" does not appear in the STADIC BuildingControl File.", "The \"data_folder\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setDataFolder(sVal.get());
        sVal.reset();
    }

    //******************
    //Site Information
    //******************
    dVal=getDouble(json, "ground_reflectance", "The key \"ground_reflectance\" does not appear in the STADIC BuildingControl File.", "The \"ground_reflectance\" is not a double.", Severity::Warning);
    if (!dVal){
        STADIC_LOG(Severity::Info, "A default value of 0.2 will be applied for the ground reflectance.");
        setGroundReflect(0.2);
    }else{
        setGroundReflect(dVal.get());
        dVal.reset();
    }

    sVal=getString(json, "wea_data_file", "The key \"wea_data_file\" does not appear in the STADIC BuildingControl File.", "The \"wea_data_file\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setWeaDataFile(sVal.get());
        sVal.reset();
    }

    iVal=getInt(json, "first_day", "The key \"first_day\" does not appear in the STADIC BuildingControl File.", "The \"first_day\" is not an integer.", Severity::Warning);
    if (!iVal){
        STADIC_LOG(Severity::Info, "The default first day will be set to 1 (Sunday).");
        setFirstDay(1);
    }else{
        setFirstDay(iVal.get());
        iVal.reset();
    }


    //******************
    //Geometry Information
    //******************
    sVal=getString(json, "material_file", "The key \"material_file\" does not appear in the STADIC BuildingControl File.", "The \"material_file\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setMatFile(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "geometry_file", "The key \"geometry_file\" does not appear in the STADIC BuildingControl File.", "The \"geometry_file\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setGeoFile(sVal.get());
    }

    dVal=getDouble(json, "building_rotation", "The key \"building_rotation\" does not appear in the STADIC BuildingControl File.", "The \"building_rotation\" is not a double.", Severity::Warning);
    if (!dVal){
        STADIC_LOG(Severity::Info, "The default building rotation will be set to 0");
        setBuildingRotation(0);
    }else{
        setBuildingRotation(dVal.get());
        dVal.reset();
    }

    sVal=getString(json, "analysis_points", "The key \"analysis_points\" does not appear in the STADIC BuildingControl File.", "The \"analysis_points\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setPTSFile(sVal.get());
        sVal.reset();
    }

    treeVal=getArray(json, "window_groups", "The key \"window_groups\" does not appear in the STADIC BuildingControl File.", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        for(auto &v : treeVal.get()){
            WindowGroup WG;
            if (WG.parseJson(v)){
                m_WindowGroups.push_back(WG);
            }else{
                return false;
            }
        }
        treeVal.reset();
    }

    sVal=getString(json, "import_units", "The key \"import_units\" does not appear in the STADIC BuildingControl File.", "The \"import_units\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        if (!setImportUnits(sVal.get())){
            return false;
        }
        sVal.reset();
    }

    sVal=getString(json, "illum_units", "The key \"illum_units\" does not appear in the STADIC BuildingControl File.", "The \"illum_units\" is not a string.", Severity::Warning);
    if (!sVal){
        STADIC_LOG(Severity::Info, "The default illuminance units will be set to lux.");
        setIllumUnits("lux");
    }else{
        setIllumUnits(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "display_units", "The key \"display_units\" does not appear in the STADIC BuildingControl File.", "The \"display_units\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        if (!setDisplayUnits(sVal.get())){
            return false;
        }
        sVal.reset();
    }

    sVal=getString(json, "occupancy_schedule", "The key \"occupancy_schedule\" does not appear in the STADIC BuildingControl File.", "The \"occupancy_schedule\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setOccSchedule(sVal.get());
        sVal.reset();
    }

    dVal=getDouble(json, "target_illuminance", "The key \"target_illuminance\" does not appear in the STADIC BuildingControl File.", "The \"target_illuminance\" is not a double.", Severity::Error);
    if (!dVal){
        return false;
    }else{
        if (!setTargetIlluminance(dVal.get())){
            return false;
        }
        dVal.reset();
    }
 

    //******************
    //Simulation Settings
    //******************
    iVal=getInt(json, "sun_divisions", "The key \"sun_divisions\" does not appear in the STADIC BuildingControl File.", "The \"sun_divisions\" is not an integer.", Severity::Warning);
    if (!iVal){
        STADIC_LOG(Severity::Info, "Sun Divisions will be set to 3.");
        setSunDivisions(3);
    }else{
        if (!setSunDivisions(iVal.get())){
            STADIC_LOG(Severity::Info, "Sun Divisions will be set to 3.");
            setSunDivisions(3);
        }
        iVal.reset();
    }

    iVal=getInt(json, "sky_divisions", "The key \"sky_divisions\" does not appear in the STADIC BuildingControl File.", "The \"sky_divisions\" is not an integer.", Severity::Warning);
    if (!iVal){
        STADIC_LOG(Severity::Info, "Sky Divisions will be set to 3.");
        setSkyDivisions(3);
    }else{
        if (!setSkyDivisions(iVal.get())){
            STADIC_LOG(Severity::Info, "Sky Divisions will be set to 3.");
            setSkyDivisions(3);
        }
        iVal.reset();
    }

    bVal=getBool(json, "daylight_savings_time", "The key \"daylight_savings_time\" does not appear in the STADIC BuildingControl File.", "The \"daylight_savings_time\" is not a boolean.", Severity::Warning);
    if (!bVal){
        STADIC_LOG(Severity::Info, "Daylight Savings Time will be enabled.");
        setDaylightSavingsTime(true);
    }else{
        setDaylightSavingsTime(bVal.get());
        bVal.reset();
    }

    treeVal=getObject(json, "radiance_parameters", "The key \"radiance_parameters\" does not appear in the STADIC BuildingControl File.", Severity::Info);
    if (!treeVal){
        STADIC_LOG(Severity::Info, "The default radiance parameters will be set.");
        if (!setDefaultRadianceParameters()){
            return false;
        }
    }else{
        iVal=getInt(treeVal.get(), "ab", "The parameter \"ab\" was not found under radiance_parameters.", "The parameter \"ab\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 4 will be applied for \"ab\".");
            setAB(4);
        }else{
            if (!setAB(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 4 will be applied for \"ab\".");
                setAB(4);
            }
            iVal.reset();
        }

        iVal=getInt(treeVal.get(), "ad", "The parameter \"ad\" was not found under radiance_parameters.", "The parameter \"ad\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 300 will be applied for \"ad\".");
            setAD(300);
        }else{
            if (!setAD(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 300 will be applied for \"ad\".");
                setAD(300);
            }
            iVal.reset();
        }

        iVal=getInt(treeVal.get(), "as", "The parameter \"as\" was not found under radiance_parameters.", "The parameter \"as\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 20 will be applied for \"as\".");
            setAS(20);
        }else{
            if (!setAS(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 20 will be applied for \"as\".");
                setAS(20);
            }
            iVal.reset();
        }

        iVal=getInt(treeVal.get(), "ar", "The parameter \"ar\" was not found under radiance_parameters.",  "The parameter \"ar\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 150 will be applied for \"ar\".");
            setAR(150);
        }else{
            if (!setAR(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 150 will be applied for \"ar\".");
                setAR(150);
            }
            iVal.reset();
        }

        dVal=getDouble(treeVal.get(), "aa", "The parameter \"aa\" was not found under radiance_parameters.", "The parameter \"aa\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 0.1 will be applied for \"aa\".");
            setAA(0.1);
        }else{
            if (!setAA(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 0.1 will be applied for \"aa\".");
                setAA(0.1);
            }
            dVal.reset();
        }
        iVal=getInt(treeVal.get(), "lr", "The parameter \"lr\" was not found under radiance_parameters.", "The parameter \"lr\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 6 will be applied for \"lr\".");
            setLR(6);
        }else{
            if (!setLR(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 6 will be applied for \"lr\".");
                setLR(6);
            }
            iVal.reset();
        }

        dVal=getDouble(treeVal.get(), "st", "The parameter \"st\" was not found under radiance_parameters.", "The parameter \"st\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 0.15 will be applied for \"st\".");
            setST(0.15);
        }else{
            if (!setST(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 0.15 will be applied for \"st\".");
                setST(0.15);
            }
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "sj", "The parameter \"sj\" was not found under radiance_parameters.", "The parameter \"sj\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 1.0 will be applied for \"sj\".");
            setSJ(1.0);
        }else{
            if (!setSJ(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 1.0 will be applied for \"sj\".");
                setSJ(1.0);
            }
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "lw", "The parameter \"lw\" was not found under radiance_parameters.", "The parameter \"lw\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 0.004 will be applied for \"lw\".");
            setLW(0.004);
        }else{
            if (!setLW(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 0.004 will be applied for \"lw\".");
                setLW(0.004);
            }
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "dj", "The parameter \"dj\" was not found under radiance_parameters.", "The parameter \"dj\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 0.0 will be applied for \"dj\".");
            setDJ(0.0);
        }else{
            if (!setDJ(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 0.0 will be applied for \"dj\".");
                setDJ(0.0);
            }
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "ds", "The parameter \"ds\" was not found under radiance_parameters.", "The parameter \"ds\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 0.200 will be applied for \"ds\".");
            setDS(0.200);
        }else{
            if (!setDS(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 0.200 will be applied for \"ds\".");
                setDS(0.200);
            }
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "dr", "The parameter \"dr\" was not found under radiance_parameters.", "The parameter \"dr\" is not a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "A default value of 2 will be applied for \"dr\".");
            setDR(2);
        }else{
            if (!setDR(dVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 2 will be applied for \"dr\".");
                setDR(2);
            }
            dVal.reset();
        }

        iVal=getInt(treeVal.get(), "dp", "The parameter \"dp\" was not found under radiance_parameters.", "The parameter \"dp\" is not a number.", Severity::Warning);
        if (!iVal){
            STADIC_LOG(Severity::Info, "A default value of 512 will be applied for \"dp\".");
            setDP(512);
        }else{
            if (!setDP(iVal.get())){
                STADIC_LOG(Severity::Info, "A default value of 512 will be applied for \"dp\".");
                setDP(512);
            }
            iVal.reset();
        }
    }
    treeVal.reset();



    //******************
    //Lighting BuildingControl
    //******************
    treeVal=getObject(json, "control_zones", "The key \"control_zones\" does not appear in the STADIC BuildingControl File.", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        for(auto &v : treeVal.get()){
            ControlZone zone;
            if (zone.parseJson(v)){
                m_ControlZones.push_back(zone);
            }else{
                return false;
            }
        }
        treeVal.reset();
    }

    //******************
    //Metrics
    //******************
    treeVal=getObject(json, "sDA", "The key \"sDA\" does not appear in the STADIC BuildingControl File.", Severity::Info);
    if (treeVal){
        double illum, frac, startTime, endTime;
        dVal=getDouble(treeVal.get(), "illuminance", "The key \"illuminance\" is missing under sDA.", "The key \"illuminance\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 300 will be used for sDA illuminance.");
            illum=300;
        }else{
            illum=dVal.get();
        }
        dVal.reset();
        dVal=getDouble(treeVal.get(), "DA_fraction", "The key \"DA_fraction\" is missing under sDA.", "The key \"DA_fraction\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 0.50 will be used for sDA fraction.");
            frac=0.50;
        }else{
            frac=dVal.get();
        }
        dVal.reset();
        dVal=getDouble(treeVal.get(), "start_time", "The key \"start_time\" is missing under sDA.", "The key \"start_time\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 8 will be used for sDA start time.");
            startTime=8;
        }else{
            startTime=dVal.get();
        }
        dVal.reset();
        dVal=getDouble(treeVal.get(), "end_time", "The key \"end_time\" is missing under sDA.", "The key \"end_time\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 17 will be used for sDA end time.");
            endTime=17;
        }else{
            endTime=dVal.get();
        }
        if (!setsDA(true, illum, frac, startTime, endTime)){
            return false;
        }
    }
    treeVal.reset();

    treeVal=getObject(json, "occupied_sDA", "The key \"occupied_sDA\" does not appear in the STADIC BuildingControl File.", Severity::Info);
    if (treeVal){
        double illum, frac;
        dVal=getDouble(treeVal.get(), "illuminance", "The key \"illuminance\" is missing under occupied_sDA.", "The key \"illuminance\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 300 will be used for occupied_sDA illuminance.");
            illum=300;
        }else{
            illum=dVal.get();
        }
        dVal.reset();
        dVal=getDouble(treeVal.get(), "DA_fraction", "The key \"DA_fraction\" is missing under occupied_sDA.", "The key \"DA_fraction\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 0.50 will be used for occupied_sDA fraction.");
            frac=0.50;
        }else{
            frac=dVal.get();
        }
        dVal.reset();
        if (!setOccsDA(true, illum, frac)){
            return false;
        }
    }
    treeVal.reset();

    dVal=getDouble(json, "DA", "The key \"DA\" does not appear in the STADIC BuildingControl File.", "The key \"DA\" is not a double.", Severity::Info);
    if (dVal){
        if (!setDA(true, dVal.get())){
            return false;
        }
    }
    dVal.reset();

    dVal=getDouble(json, "cDA", "The key \"cDA\" does not appear in the STADIC BuildingControl File.", "The key \"cDA\" is not a double.", Severity::Info);
    if (dVal){
        if (!setcDA(true, dVal.get())){
            return false;
        }
    }
    dVal.reset();

    bVal=getBool(json, "DF", "The key \"DF\" does not appear in the STADIC BuildingControl File.", "The key \"DF\" is not a boolean.", Severity::Info);
    if (bVal){
        setDF(bVal.get());
    }
    bVal.reset();

    treeVal=getObject(json, "UDI", "The key \"UDI\" does not appear in the STADIC BuildingControl File.", Severity::Info);
    if (treeVal){
        double minimum, maximum;
        dVal=getDouble(treeVal.get(), "minimum", "The key \"minimum\" is missing under UDI.", "The key \"minimum\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 100 will be used for UDI minimum.");
            minimum=100;
        }else{
            minimum=dVal.get();
        }
        dVal.reset();
        dVal=getDouble(treeVal.get(), "maximum", "The key \"maximum\" is missing under UDI.", "The key \"maximum\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 2500 will be used for UDI maximum.");
            maximum=2500;
        }else{
            maximum=dVal.get();
        }
        dVal.reset();
        if (!setUDI(true, minimum, maximum)){
                return false;
        }
    }
    treeVal.reset();
    return true;
}


}