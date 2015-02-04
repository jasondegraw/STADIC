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
#include "spacecontrol.h"

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
void BuildingControl::resetWeaDataFile(){
    m_WeaDataFile.reset();
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
void BuildingControl::resetFirstDay(){
    m_FirstDay.reset();
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
void BuildingControl::resetBuildingRotation(){
    m_BuildingRotation.reset();
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
void BuildingControl::resetImportUnits(){
    m_ImportUnits.reset();
}

void BuildingControl::setIllumUnits(std::string units){
    if (units=="lux" || units=="fc"){
        m_IllumUnits=units;
    }else{
        STADIC_WARNING("The illuminance units must be either \"lux\" or \"fc\".  The default illuminance units will be set to lux.");
        m_IllumUnits="lux";
    }
}
void BuildingControl::resetIllumUnits(){
    m_IllumUnits.reset();
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
void BuildingControl::resetDisplayUnits(){
    m_DisplayUnits.reset();
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
void BuildingControl::resetTargetIlluminance(){
    m_TargetIlluminance.reset();
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
void BuildingControl::resetSunDivisions(){
    m_SunDivisions.reset();
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
void BuildingControl::resetSkyDivisions(){
    m_SkyDivisions.reset();
}

void BuildingControl::setDaylightSavingsTime(bool value)
{
    m_DaylightSavingsTime=value;
}
void BuildingControl::resetDaylightSavingsTime(){
    m_DaylightSavingsTime.reset();
}

//Getters
//******************
//General Information
//******************
boost::optional<std::string> BuildingControl::weaDataFile(){
    return m_WeaDataFile;
}
boost::optional<int> BuildingControl::firstDay(){
    return m_FirstDay;
}
boost::optional<double> BuildingControl::buildingRotation(){
    return m_BuildingRotation;
}
std::vector<std::shared_ptr<Control>> BuildingControl::spaces(){
    return m_Spaces;
}
boost::optional<std::string> BuildingControl::importUnits(){
    return m_ImportUnits;
}
boost::optional<std::string> BuildingControl::illumUnits(){
    return m_IllumUnits;
}
boost::optional<std::string> BuildingControl::displayUnits(){
    return m_DisplayUnits;
}
boost::optional<double> BuildingControl::targetIlluminance(){
    return m_TargetIlluminance;
}
boost::optional<int> BuildingControl::sunDivisions(){
    return m_SunDivisions;
}
boost::optional<int> BuildingControl::skyDivisions(){
    return m_SkyDivisions;
}

boost::optional<bool> BuildingControl::daylightSavingsTime(){
    return m_DaylightSavingsTime;
}
boost::optional<std::string> BuildingControl::getRadParam(std::string parameterSet, std::string parameterName){
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>>::const_iterator got=m_RadParams.find(parameterSet);
    if (got==m_RadParams.end()){
        return boost::none;
    }
    std::unordered_map<std::string, std::string>::const_iterator got2=m_RadParams[parameterSet].find(parameterName);
    if (got2==m_RadParams[parameterSet].end()){
        return boost::none;
    }
    return boost::optional<std::string>(m_RadParams[parameterSet][parameterName]);
}
boost::optional<std::unordered_map<std::string, std::unordered_map<std::string, std::string>>> BuildingControl::getAllRadParams(){
    return m_RadParams;
}

//******************
//PARSER
//******************

bool BuildingControl::parseJson(const std::string &file)
{
    boost::optional<JsonObject> jsonOpt = readJsonDocument(file);

    if(!jsonOpt){
        STADIC_LOG(Severity::Fatal, "Failed to read json input file \"" + file + "\" .");
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
    sVal=getString(treeVal.get(), "import_units");
    if (sVal){
        setImportUnits(sVal.get());
        sVal.reset();
    }
    //Display Units
    sVal=getString(treeVal.get(), "display_units");
    if (sVal){
        setDisplayUnits(sVal.get());
        sVal.reset();
    }
    //Illuminance Units
    sVal=getString(treeVal.get(), "illum_units");
    if (sVal){
        setIllumUnits(sVal.get());
        sVal.reset();
    }
    //Radiance Parameters
    boost::optional<JsonObject> radTree;
    radTree=getObject(treeVal.get(), "radiance_parameters");
    if (radTree){
        for (std::string setName : radTree.get().getMemberNames()){
            boost::optional<JsonObject> tempTree;
            tempTree=getObject(radTree.get(), setName, "The key \""+setName+ "\"does not appear in the STADIC Control File.", Severity::Fatal);
            std::pair<std::string, std::unordered_map<std::string, std::string>> tempPair=std::make_pair (setName, std::unordered_map<std::string, std::string> ());
            m_RadParams.insert(tempPair);
            //Added to make radiance_parameters work with jsonCPP
            for (Json::Value::iterator it =tempTree.get().begin(); it != tempTree.get().end(); it++){
                Json::Value key =it.key();
                Json::Value value = (*it);
                std::pair<std::string, std::string> parameters (key.asString(), value.asString());
                m_RadParams[setName].insert(parameters);
            }
            /* //Removed to make radiance_parameters work with jsonCPP
            for (std::string paramName : tempTree.get().getMemberNames()){
                sVal=getString(tempTree.get(), paramName, "The key \""+paramName+ "\" does not appear in the STADIC Control File.", "The key \""+paramName+"\" does not appear in the STADIC Control File.", Severity::Fatal);
                std::pair<std::string, std::string> parameters (paramName, sVal.get());
                m_RadParams[setName].insert(parameters);
            }
            */
        }

    }
    //Daylight Savings Time
    bVal=getBool(treeVal.get(), "daylight_savings_time");
    if (!bVal){
        STADIC_LOG(Severity::Info, "Daylight Savings Time will not be enabled for this simulation.");
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
    //TargetIlluminance
    dVal=getDouble(treeVal.get(), "target_illuminance");
    if (dVal){
        if (setTargetIlluminance(dVal.get())){
            dVal.reset();
        }
    }
    //Sky Divisions
    iVal=getInt(treeVal.get(), "sky_divisions");
    if (iVal){
        if (setSkyDivisions(iVal.get())){
            iVal.reset();
        }
    }
    //Sun Divisions
    iVal=getInt(treeVal.get(), "sun_divisions");
    if (iVal){
        if (setSunDivisions(iVal.get())){
            iVal.reset();
        }
    }

    //******************
    //Loop over Spaces
    //******************
    treeVal=getArray(json, "spaces", "The key \"spaces\" does not appear in the STADIC BuildingControl File.", Severity::Error);
    if (!treeVal){
        //return false;
    }else{
        for(auto &v : treeVal.get()){
            // This is not a pretty way to do this, but it shouldn't lead to leaks
            std::shared_ptr<Control> spaceControl(new Control);
            if (spaceControl->parseJson(v, this)){
                m_Spaces.push_back(spaceControl);
            }else{
                return false;
            }
        }
        treeVal.reset();
    }

    return true;
}


}
