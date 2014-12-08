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
//General Information
//******************
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

bool BuildingControl::setBuildingRotation(double value){
    if (value>360 || value<-360){
        STADIC_ERROR("The building rotation must be between -360 and 360.");
        return false;
    }else{
        m_BuildingRotation=value;
    }
    return true;
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

//Getters
//******************
//General Information
//******************
std::string BuildingControl::weaDataFile(){
    return m_WeaDataFile;
}
int BuildingControl::firstDay(){
    return m_FirstDay;
}
double BuildingControl::buildingRotation(){
    return m_BuildingRotation;
}
std::vector<Control> BuildingControl::spaces(){
    return m_Spaces;
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
double BuildingControl::targetIlluminance(){
    return m_TargetIlluminance;
}
int BuildingControl::sunDivisions(){
    return m_SunDivisions;
}
int BuildingControl::skyDivisions(){
    return m_SkyDivisions;
}

bool BuildingControl::daylightSavingsTime(){
    return m_DaylightSavingsTime;
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

    treeVal=getObject(json, "general", "The key \"general\" does not appear in the STADIC Control File.", Severity::Fatal);
    //Import Units
    sVal=getString(treeVal.get(), "import_units", "The key \"import_units\" does not appear in the STADIC Control File.", "The key \"import_units\" does not contain a string.", Severity::Warning);
    if (!setImportUnits(sVal.get())){
        return false;
    }
    sVal.reset();
    //Display Units
    sVal=getString(treeVal.get(), "display_units", "The key \"display_units\" does not appear in the STADIC Control File.", "The key \"display_units\" does not contain a string.", Severity::Warning);
    if (!setDisplayUnits(sVal.get())){
        return false;
    }
    sVal.reset();
    //Illuminance Units
    sVal=getString(treeVal.get(), "illum_units", "The key \"illum_units\" does not appear in the STADIC Control File.", "The key \"illum_units\" does not contain a string.", Severity::Warning);
    setIllumUnits(sVal.get());
    sVal.reset();
    //Radiance Parameters
    boost::optional<JsonObject> radTree;
    radTree=getObject(treeVal.get(), "radiance_parameters", "The key \"radiance_parameters\" does not appear in the STADIC Control File.", Severity::Warning);
    if (!radTree){
        return false;
    }
    for (std::string setName : radTree.get().getMemberNames()){
        boost::optional<JsonObject> tempTree;
        tempTree=getObject(radTree.get(), setName, "The key \""+setName+ "\"does not appear in the STADIC Control File.", Severity::Fatal);
        std::pair<std::string, std::unordered_map<std::string, std::string>> tempPair=std::make_pair (setName, std::unordered_map<std::string, std::string> ());
        m_RadParams.insert(tempPair);
        for (std::string paramName : tempTree.get().getMemberNames()){
            sVal=getString(tempTree.get(), paramName, "The key \""+paramName+ "\" does not appear in the STADIC Control File.", "The key \""+paramName+"\" does not appear in the STADIC Control File.", Severity::Fatal);
            std::pair<std::string, std::string> parameters (paramName, sVal.get());
            m_RadParams[setName].insert(parameters);
        }
    }
    //Daylight Savings Time
    bVal=getBool(treeVal.get(), "daylight_savings_time", false, "The key \"daylight_savings_time\" does not appear in the STADIC Control File.", Severity::Warning);
    if (!bVal){
        setDaylightSavingsTime(false);
    }else{
        setDaylightSavingsTime(bVal.get());
        bVal.reset();
    }
    //Weather data
    sVal=getString(treeVal.get(), "epw_file", "The key \"epw_file\" does not appear in the STADIC Control File.", "The key \"epw_file\" does not contain a string.", Severity::Fatal);
    setWeaDataFile(sVal.get());
    sVal.reset();
    //FirstDay
    iVal=getInt(treeVal.get(), "first_day", "The key \"first_day\" does not appear in the STADIC Control File.  The start day for the weather file will be used.", "The key \"first_day\" contains a bad value.", Severity::Warning);
    if (iVal){
        if (!setFirstDay(iVal.get())){
            STADIC_LOG(Severity::Warning, "The key \"first_day\" contains an invalid entry.");
            setFirstDay(1);
        }
        iVal.reset();
    }
    //Building Rotation
    dVal=getDouble(treeVal.get(), "building_rotation", "The key \"building_rotation\" does not appear in the STADIC Control File.  A value of 0 will be assumed.", "The key\"building_rotation\" contains a value that is not a double.  A value of 0 will be assumed.", Severity::Warning);
    if (dVal){
        if (!setBuildingRotation(dVal.get())){
            STADIC_LOG(Severity::Warning, "The key \"building_rotation\" contains an invalid entry.  A value of 0 will be assumed.");
        }
        dVal.reset();
    }

    treeVal=getObject(json, "spaces", "The key \"spaces\" does not appear in the STADIC Control File.", Severity::Fatal);

    //******************
    //Loop over Spaces
    //******************
    treeVal=getArray(json, "spaces", "The key \"window_groups\" does not appear in the STADIC BuildingControl File.", Severity::Error);
    if (!treeVal){
        return false;
    }else{
        for(auto &v : treeVal.get()){
            Control spaceControl;
            if (spaceControl.parseJson(v)){
                m_Spaces.push_back(spaceControl);
            }else{
                return false;
            }
        }
        treeVal.reset();
    }


    /*
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
    */
    return true;
}


}
