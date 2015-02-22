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

#include "spacecontrol.h"
#include "windowgroup.h"
#include <iostream>
#include <fstream>
#include <string>
#include "functions.h"

namespace stadic {

Control::Control()
{
}

//Setters
//******************
//Folder Information
//******************
void Control::setSpaceName(std::string name){
    m_SpaceName=name;
}
void Control::setSpaceDirectory(std::string directory){
    m_SpaceDirectory=directory;
}
void Control::setGeoDirectory(std::string directory){
    m_GeoDirectory=directory;
}
void Control::setIESDirectory(std::string directory){
    m_IESDirectory=directory;
}
void Control::setResultsDirectory(std::string directory){
    m_ResultsDirectory=directory;
}
void Control::setInputDirectory(std::string directory){
    m_InputDirectory=directory;
}

//******************
//Site Information
//******************
bool Control::setGroundReflect(double value){
    if (value>1 || value<0){
        STADIC_WARNING("The ground reflectance must be between 0 and  1.\n\tA default value of 0.2 will be applied.");
        //std::cerr<<"WARNING: The ground reflectance must be between 0 and  1.\n\tA default value of 0.2 will be applied."<<std::endl;
        m_GroundReflect=0.2;
    }else{
        m_GroundReflect=value;
    }
    return true;
}
void Control::setWeaFile(std::string fileName){
    m_WeaFile=fileName;
}


//******************
//Geometry Information
//******************
void Control::setMatFile(std::string file){
    m_MatFile=file;
}
void Control::setGeoFile(std::string file){
    m_GeoFile=file;
}
void Control::setBuildingRotation(double value){
    m_BuildingRotation=value;
}
void Control::setPTSFile(std::vector<std::string> files){
    m_PTSFile=files;
}
void Control::setXSpacing(std::string value){
    m_XSpacing=value;
}

void Control::setYSpacing(std::string value){
    m_YSpacing=value;
}

void Control::setOffset(std::string value){
    m_Offset=value;
}

void Control::setZOffset(std::string value){
    m_ZOffset=value;
}

void Control::setIdentifiers(std::vector<std::string> identifiers){
    m_Identifiers=identifiers;
}

void Control::setModifiers(std::vector<std::string> modifiers){
    m_Modifiers=modifiers;
}

void Control::setOccSchedule(std::string file){
    m_OccSchedule=file;
}
void Control::setLightSchedule(std::string file){
    m_LightSchedule=file;
}

bool Control::setTargetIlluminance(double value){
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
bool Control::setSunDivisions(int value){
    if (value<1 || value>6){
        STADIC_WARNING("The sun divisions must be between 1 and 6.\n\tA default value of 3 will be applied.");
        m_SunDivisions=3;
    }else{
        m_SunDivisions=value;
    }
    return true;
}

bool Control::setSkyDivisions(int value)
{
    if (value<1 || value>6){
        STADIC_WARNING("The sky divisions must be between 1 and 6.\n\tA default value of 3 will be applied.");
        m_SkyDivisions=3;
    }else{
        m_SkyDivisions=value;
    }
    return true;
}
void Control::setFirstDay(boost::optional<int> value){
    m_FirstDay=value;
}

void Control::setImportUnits(std::string units){
    m_ImportUnits=units;
}

void Control::setIllumUnits(std::string units){
    m_IllumUnits=units;
}

void Control::setDisplayUnits(std::string units){
    m_displayUnits=units;
}

void Control::setDaylightSavingsTime(bool DST){
    m_DaylightSavingsTime=DST;
}

//******************
//Metrics
//******************
bool Control::setDA(bool run, double illum){
    m_DA=run;
    if (illum>0){
        m_DAIllum=illum;
    }else{
        STADIC_ERROR("The DA illuminance must be greater than 0.");
        return false;
    }
    return true;
}
bool Control::setcDA(bool run, double illum){
    m_cDA=run;
    if (illum>0){
        m_cDAIllum=illum;
    }else{
        STADIC_ERROR("The cDA illuminance must be greater than 0.");
        return false;
    }
    return true;
}
bool Control::setsDA(bool run, double illum, double DAFrac, double startTime, double endTime){
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
bool Control::setOccsDA(bool run, double illum, double DAFrac){
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
void Control::setDF(bool run){
    m_DF=run;
}
bool Control::setUDI(bool run, double minIllum, double maxIllum){
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
std::string Control::spaceName(){
    return m_SpaceName;
}
std::string Control::spaceDirectory(){
    return m_SpaceDirectory;
}
std::string Control::geoDirectory(){
    return m_GeoDirectory;
}
std::string Control::iesDirectory(){
    return m_IESDirectory;
}
std::string Control::resultsDirectory(){
    return m_ResultsDirectory;
}
std::string Control::inputDirectory(){
    return m_InputDirectory;
}
std::string Control::intermediateDataDirectory(){
    return m_ResultsDirectory +"intermediateData\\";
}

//******************
//Site Information
//******************
double Control::groundReflect(){
    return m_GroundReflect;
}
std::string Control::weaFile(){
    return m_WeaFile;
}

//******************
//Geometry Information
//******************
std::string Control::matFile(){
    return m_MatFile;
}
std::string Control::geoFile(){
    return m_GeoFile;
}
double Control::buildingRotation(){
    return m_BuildingRotation;
}
std::vector<std::string> Control::ptsFile(){
    return m_PTSFile;
}
boost::optional<std::string> Control::xSpacing(){
    return m_XSpacing;
}

boost::optional<std::string> Control::ySpacing(){
    return m_YSpacing;
}

boost::optional<std::string> Control::offset(){
    return m_Offset;
}

boost::optional<std::string> Control::zOffset(){
    return m_ZOffset;
}

boost::optional<std::vector<std::string>> Control::identifiers(){
    return m_Identifiers;
}

boost::optional<std::vector<std::string>> Control::modifiers(){
    return m_Modifiers;
}

std::vector<WindowGroup> Control::windowGroups(){
    return m_WindowGroups;
}
std::string Control::occSchedule(){
    return m_OccSchedule;
}
std::string Control::lightSchedule(){
    return m_LightSchedule;
}

double Control::targetIlluminance(){
    return m_TargetIlluminance;
}

//******************
//Simulation Settings
//******************
int Control::sunDivisions(){
    return m_SunDivisions;
}
int Control::skyDivisions(){
    return m_SkyDivisions;
}
boost::optional<std::string> Control::getRadParam(std::string parameterSet, std::string parameterName){
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

boost::optional<std::unordered_map<std::string, std::string>> Control::getParamSet(std::string setName){
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>>::const_iterator got=m_RadParams.find(setName);
    if (got==m_RadParams.end()){
        return boost::none;
    }

    return boost::optional<std::unordered_map<std::string, std::string>>(m_RadParams[setName]);
}
boost::optional<int> Control::firstDay(){
    return m_FirstDay;
}

std::string Control::importUnits(){
    return m_ImportUnits;
}

std::string Control::illumUnits(){
    return m_IllumUnits;
}

std::string Control::displayUnits(){
    return m_displayUnits;
}

bool Control::daylightSavingsTime(){
    return m_DaylightSavingsTime;
}



//******************
//Lighting Control
//******************
std::vector<ControlZone> Control::controlZones(){
    return m_ControlZones;
}

//******************
//Metrics
//******************
bool Control::runDA(){
    return m_DA;
}
double Control::DAIllum(){
    return m_DAIllum;
}
bool Control::runsDA(){
    return m_sDA;
}
double Control::sDAIllum(){
    return m_sDAIllum;
}
double Control::sDAFrac(){
    return m_sDAFrac;
}
double Control::sDAStart(){
    return m_sDAStart;
}
double Control::sDAEnd(){
    return m_sDAEnd;
}
bool Control::runOccsDA(){
    return m_OccsDA;
}
double Control::occsDAIllum(){
    return m_OccsDAIllum;
}
double Control::occsDAFrac(){
    return m_OccsDAFrac;
}
bool Control::runcDA(){
    return m_cDA;
}
double Control::cDAIllum(){
    return m_cDAIllum;
}
bool Control::runDF(){
    return m_DF;
}
bool Control::runUDI(){
    return m_UDI;
}
double Control::UDIMin(){
    return m_UDIMin;
}
double Control::UDIMax(){
    return m_UDIMax;
}





//******************
//PARSER
//******************

bool Control::parseJson(const JsonObject &json, BuildingControl *buildingControl)
{
    if (json.empty()){
        STADIC_LOG(Severity::Error, "The space does not contain data.");
        return false;
    }

    //get_value_or(/*default*/);
    boost::optional<std::string> sVal;
    boost::optional<double> dVal;
    boost::optional<int> iVal;
    boost::optional<bool> bVal;
    boost::optional<JsonObject> treeVal;

    setWeaFile(buildingControl->weaDataFile().get());
    if (buildingControl->buildingRotation()){
        setBuildingRotation(buildingControl->buildingRotation().get());
    }
    setFirstDay(buildingControl->firstDay());
    setImportUnits(buildingControl->importUnits().get());
    setIllumUnits(buildingControl->illumUnits().get());
    setDisplayUnits(buildingControl->displayUnits().get());
    if (buildingControl->daylightSavingsTime()){
        setDaylightSavingsTime(buildingControl->daylightSavingsTime().get());
    }else{
        setDaylightSavingsTime(false);
    }

    //******************
    //Folder Information
    //******************
    sVal=getString(json, "space_name", "The key \"space_name\" does not appear in the STADIC Control File.", "The key \"space_name\" does not contain a string.", Severity::Error);
    if(!sVal){
        return false;
    }else{
        setSpaceName(sVal.get());
        sVal.reset();
    }


    sVal=getString(json, "space_directory", "The key \"space_directory\" does not appear in the STADIC Control File.", "The key \"space_directory\" does not contain a string.", Severity::Error);
    if(!sVal){
        return false;
    }else{
        setSpaceDirectory(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "geometry_directory", "The key \"geometry_directory\" does not appear in the STADIC Control File.", "The \"geometry_directory\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setGeoDirectory(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "ies_directory", "The key \"ies_directory\" does not appear in the STADIC Control File.", "The \"ies_directory\" is not a string.", Severity::Warning);
    if (sVal){
        setIESDirectory(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "results_directory", "The key \"results_directory\" does not appear in the STADIC Control File.", "The \"results_directory\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setResultsDirectory(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "input_directory", "The key \"input_directory\" does not appear in the STADIC Control File.", "The \"input_directory\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setInputDirectory(sVal.get());
        sVal.reset();
    }

    //******************
    //Site Information
    //******************
    dVal=getDouble(json, "ground_reflectance");
    if (!dVal){
        STADIC_LOG(Severity::Info, "A default value of 0.2 will be applied for the ground reflectance.");
        setGroundReflect(0.2);
    }else{
        setGroundReflect(dVal.get());
        dVal.reset();
    }


    //******************
    //Geometry Information
    //******************
    sVal=getString(json, "material_file", "The key \"material_file\" does not appear in the STADIC Control File.", "The \"material_file\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setMatFile(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "geometry_file", "The key \"geometry_file\" does not appear in the STADIC Control File.", "The \"geometry_file\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setGeoFile(sVal.get());
        sVal.reset();
    }

    //Analysis Points
    treeVal=getObject(json, "analysis_points", "The key \"analysis_points\" does not appear in the STADIC Control File.", Severity::Fatal);
    //files
    boost::optional<JsonObject> list;
    list=getArray(treeVal.get(), "files", "The key \"files\" does not appear within \"analysis_points\" in the STADIC Control File.", Severity::Fatal);
    std::vector<std::string> tempVec;
    if (list.get().size()<1){
        STADIC_LOG(Severity::Fatal, "No analysis points file has been listed for the space named \""+m_SpaceName+"\"");
    }
    for (unsigned index=0;index<list.get().size();index++){
        tempVec.push_back(list.get()[index].asString());
    }
    setPTSFile(tempVec);
    list.reset();
    //x-spacing
    sVal=getString(treeVal.get(), "x_spacing");
    if (sVal){
        setXSpacing(sVal.get());
        sVal.reset();
    }
    //y-spacing
    sVal=getString(treeVal.get(), "y_spacing");
    if (sVal){
        setYSpacing(sVal.get());
        sVal.reset();
    }
    //offset
    sVal=getString(treeVal.get(), "offset");
    if (sVal){
        setOffset(sVal.get());
        sVal.reset();
    }
    //z-offset
    sVal=getString(treeVal.get(), "z_offset");
    if (sVal){
        setZOffset(sVal.get());
        sVal.reset();
    }
    //identifier
    list=getArray(treeVal.get(),"identifier");
    if (list){
        tempVec.clear();
        for (unsigned index=0;index<list.get().size();index++){
            tempVec.push_back(list.get()[index].asString());
        }
        setIdentifiers(tempVec);
    }
    list.reset();
    //modifier
    list=getArray(treeVal.get(),"modifier");
    if (list){
        tempVec.clear();
        for (unsigned index=0;index<list.get().size();index++){
            tempVec.push_back(list.get()[index].asString());
        }
        setModifiers(tempVec);
    }
    list.reset();
    //Code works through this point.

    treeVal=getArray(json, "window_groups", "The key \"window_groups\" does not appear in the STADIC Control Space.", Severity::Error);
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

    sVal=getString(json, "occupancy_schedule", "The key \"occupancy_schedule\" does not appear in the STADIC Control File.", "The \"occupancy_schedule\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setOccSchedule(sVal.get());
        sVal.reset();
    }

    sVal=getString(json, "lighting_schedule","The key \"lighting_schedule\" does not appear in the STADIC Control Space.", "The \"lighting_schedule\" is not a string.", Severity::Error);
    if (!sVal){
        return false;
    }else{
        setLightSchedule(sVal.get());
        sVal.reset();
    }

    dVal=getDouble(json, "target_illuminance", "The key \"target_illuminance\" does not appear within the STADIC Control Space.  The \"general\" value will be attempted.", "The \"target_illuminance\" is not a double.  The \"general\" value will be attempted.", Severity::Warning);
    if (!dVal){
        if (buildingControl->targetIlluminance()){
            setTargetIlluminance(buildingControl->targetIlluminance().get());
            STADIC_LOG(Severity::Info, "The General target illuminance value of " +toString(buildingControl->targetIlluminance().get()) +" will be applied to the space named \"" +m_SpaceName+"\"");
        }else{
            STADIC_LOG(Severity::Fatal, "General does not contain a target illuminance.  Please add this to the control file before proceeding.");
            return false;
        }
    }else{
        if (!setTargetIlluminance(dVal.get())){
            return false;
        }
        dVal.reset();
    }
 

    //******************
    //Simulation Settings
    //******************
    iVal=getInt(json, "sun_divisions", "The key \"sun_divisions\" does not appear in the STADIC Control Space.  The \"general\" value will be attempted.", "The \"sun_divisions\" is not an integer.  The \"general\" value will be attempted.", Severity::Warning);
    if (!iVal){
        if (buildingControl->sunDivisions()){
            setSunDivisions(buildingControl->sunDivisions().get());
        }else{
            STADIC_LOG(Severity::Info, "Sun Divisions will be set to 3.");
            setSunDivisions(3);
        }
    }else{
        if (!setSunDivisions(iVal.get())){
            STADIC_LOG(Severity::Info, "Sun Divisions will be set to 3.");
            setSunDivisions(3);
        }
        iVal.reset();
    }

    iVal=getInt(json, "sky_divisions", "The key \"sky_divisions\" does not appear in the STADIC Control Space.  The \"general\" value will be attempted.", "The \"sky_divisions\" is not an integer.  The \"general\" value will be attempted.", Severity::Warning);
    if (!iVal){
        if (buildingControl->skyDivisions()){
            setSkyDivisions(buildingControl->skyDivisions().get());
        }else{
            STADIC_LOG(Severity::Info, "Sky Divisions will be set to 3.");
            setSkyDivisions(3);
        }
    }else{
        if (!setSkyDivisions(iVal.get())){
            STADIC_LOG(Severity::Info, "Sky Divisions will be set to 3.");
            setSkyDivisions(3);
        }
        iVal.reset();
    }
    //Radiance Parameters
    boost::optional<JsonObject> radTree;
    radTree=getObject(json, "radiance_parameters");
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
        }
        /* //Old parameter parsing code
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
        */
    }else{
         //This gets all of the radiance parameters.
         m_RadParams=buildingControl->getAllRadParams().get();
         /*
         for ( auto i = buildingControl->getAllRadParams().get().begin(); i != buildingControl->getAllRadParams().get().end(); ++i ){
            std::pair<std::string, std::unordered_map<std::string, std::string>> tempPair=std::make_pair (i->first, std::unordered_map<std::string, std::string> ());
            m_RadParams.insert(tempPair);
            for (auto j=i->second.begin();j!= i->second.end();++j){
                std::pair<std::string, std::string> parameters (j->first, j->second);
                m_RadParams[i->first].insert(parameters);
            }
        }
        */
    }
    //Fails to verify parameters.
    verifyParameters();

    //******************
    //Lighting Control
    //******************
    treeVal=getObject(json, "control_zones", "The key \"control_zones\" does not appear in the STADIC Control File.", Severity::Warning);
    if (treeVal){
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
    treeVal=getObject(json, "sDA", "The key \"sDA\" does not appear in the STADIC Control File.", Severity::Info);
    if (treeVal){
        double illum, frac, startTime, endTime;
        bool calculate;
        bVal=getBool(treeVal.get(), "calculate", false, "The key \"calculate\" is not a boolean.", Severity::Info);
        if (!bVal){
            calculate=false;
        }else{
            calculate=bVal.get();
            bVal.reset();
        }
        dVal=getDouble(treeVal.get(), "illuminance", "The key \"illuminance\" is missing under sDA.", "The key \"illuminance\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 300 will be used for sDA illuminance.");
            illum=300;
        }else{
            illum=dVal.get();
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "DA_fraction", "The key \"DA_fraction\" is missing under sDA.", "The key \"DA_fraction\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 0.50 will be used for sDA fraction.");
            frac=0.50;
        }else{
            frac=dVal.get();
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "start_time", "The key \"start_time\" is missing under sDA.", "The key \"start_time\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 8 will be used for sDA start time.");
            startTime=8;
        }else{
            startTime=dVal.get();
            dVal.reset();
        }
        dVal=getDouble(treeVal.get(), "end_time", "The key \"end_time\" is missing under sDA.", "The key \"end_time\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 17 will be used for sDA end time.");
            endTime=17;
        }else{
            endTime=dVal.get();
        }
        if (!setsDA(calculate, illum, frac, startTime, endTime)){
            return false;
        }
        treeVal.reset();
    }


    treeVal=getObject(json, "occupied_sDA", "The key \"occupied_sDA\" does not appear in the STADIC Control File.", Severity::Info);
    if (treeVal){
        double illum, frac;
        bool calculate;
        bVal=getBool(treeVal.get(), "calculate", false, "The key \"calculate\" is not a boolean.", Severity::Info);
        if (!bVal){
            calculate=false;
        }else{
            calculate=bVal.get();
            bVal.reset();
        }
        dVal=getDouble(treeVal.get(), "illuminance", "The key \"illuminance\" is missing under occupied_sDA.", "The key \"illuminance\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 300 will be used for occupied_sDA illuminance.");
            illum=300;
        }else{
            illum=dVal.get();
            dVal.reset();
        }

        dVal=getDouble(treeVal.get(), "DA_fraction", "The key \"DA_fraction\" is missing under occupied_sDA.", "The key \"DA_fraction\" does not contain a number.", Severity::Warning);
        if (!dVal){
            STADIC_LOG(Severity::Info, "An assumed value of 0.50 will be used for occupied_sDA fraction.");
            frac=0.50;
        }else{
            frac=dVal.get();
            dVal.reset();
        }
        if (!setOccsDA(calculate, illum, frac)){
            return false;
        }
        treeVal.reset();
    }

    treeVal=getObject(json, "DA");
    if (treeVal){
        double illuminance;
        bool calculate;
        bVal=getBool(treeVal.get(), "calculate", false, "The key \"calculate\" is not a boolean.", Severity::Info);
        if (!bVal){
            calculate=false;
        }else{
            calculate=bVal.get();
            bVal.reset();
        }
        dVal=getDouble(treeVal.get(), "illuminance", "The key \"illuminance\" does not appear in the STADIC Control File.", "The key \"illuminance\" is not a double.", Severity::Error);
        if (dVal){
            illuminance=dVal.get();
            dVal.reset();
        }
        if (!setDA(calculate, illuminance)){
            return false;
        }
        treeVal.reset();
    }

    treeVal=getObject(json, "cDA");
    if (treeVal){
        double illuminance;
        bool calculate;
        bVal=getBool(treeVal.get(), "calculate", false, "The key \"calculate\" is not a boolean.", Severity::Info);
        if (!bVal){
            calculate=false;
        }else{
            calculate=bVal.get();
            bVal.reset();
        }
        dVal=getDouble(treeVal.get(), "illuminance", "The key \"illuminance\" does not appear in the STADIC Control File.", "The key \"illuminance\" is not a double.", Severity::Info);
        if (dVal){
            illuminance=dVal.get();
            dVal.reset();
        }
        if (!setcDA(calculate, illuminance)){
            return false;
        }
        treeVal.reset();
    }

    bVal=getBool(json, "DF", "The key \"DF\" does not appear in the STADIC Control File.", "The key \"DF\" is not a boolean.", Severity::Info);
    if (bVal){
        setDF(bVal.get());
    }
    bVal.reset();

    treeVal=getObject(json, "UDI", "The key \"UDI\" does not appear in the STADIC Control File.", Severity::Info);
    if (treeVal){
        double minimum, maximum;
        bool calculate;
        bVal=getBool(treeVal.get(), "calculate", false, "The key \"calculate\" is not a boolean.", Severity::Info);
        if (!bVal){
            calculate=false;
        }else{
            calculate=bVal.get();
            bVal.reset();
        }
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
        if (!setUDI(calculate, minimum, maximum)){
                return false;
        }
        treeVal.reset();
    }
    return true;
}

bool Control::verifyParameters(){
    bool allOk=true;
    //ab
    if (!checkParameter("dmx", "ab", "int")){
        allOk=false;
    }
    if (!checkParameter("vmx", "ab", "int")){
        allOk=false;
    }
    if (!checkParameter("default", "ab", "int")){
        allOk=false;
    }
    //ad
    if (!checkParameter("dmx", "ad", "int")){
        allOk=false;
    }
    if (!checkParameter("vmx", "ad", "int")){
        allOk=false;
    }
    if (!checkParameter("default", "ad", "int")){
        allOk=false;
    }
    //as
    if (!checkParameter("dmx", "as", "int")){
        allOk=false;
    }
    if (!checkParameter("vmx", "as", "int")){
        allOk=false;
    }
    if (!checkParameter("default", "as", "int")){
        allOk=false;
    }
    //dt
    if (!checkParameter("dmx", "dt", "double")){
        allOk=false;
    }
    if (!checkParameter("vmx", "dt", "double")){
        allOk=false;
    }
    if (!checkParameter("default", "dt", "double")){
        allOk=false;
    }
    //dc
    if (!checkParameter("dmx", "dc", "double")){
        allOk=false;
    }
    if (!checkParameter("vmx", "dc", "double")){
        allOk=false;
    }
    if (!checkParameter("default", "dc", "double")){
        allOk=false;
    }
    //dj
    if (!checkParameter("dmx", "dj", "double")){
        allOk=false;
    }
    if (!checkParameter("vmx", "dj", "double")){
        allOk=false;
    }
    if (!checkParameter("default", "dj", "double")){
        allOk=false;
    }
    //dp
    if (!checkParameter("dmx", "dp", "double")){
        allOk=false;
    }
    if (!checkParameter("vmx", "dp", "double")){
        allOk=false;
    }
    if (!checkParameter("default", "dp", "double")){
        allOk=false;
    }
    //lw
    if (!checkParameter("dmx", "lw", "double")){
        allOk=false;
    }
    if (!checkParameter("vmx", "lw", "double")){
        allOk=false;
    }
    if (!checkParameter("default", "lw", "double")){
        allOk=false;
    }
    return allOk; // Just guessing here... but VS requires a return
}

bool Control::checkParameter(std::string setName, std::string parameter, std::string varType){
    boost::optional<std::string> check;
    bool ok;
    check=getRadParam(setName, parameter);
    if (check){
        if (varType=="int"){
            toInteger(check.get(), &ok);
            if (!ok){
                STADIC_LOG(Severity::Error, "The parameter "+parameter+" within the "+setName+" set in "+m_SpaceName +" is not an integer.");
            }
        }else if (varType=="double"){
            toDouble(check.get(), &ok);
            if (!ok){
                STADIC_LOG(Severity::Error, "The parameter "+parameter+" within the "+setName+" set in "+m_SpaceName +" is not a double.");
            }
        }else{
            STADIC_LOG(Severity::Fatal, "The variable type for the verification of the parameters is not a known type.  This can be either \"int\" or \"double\".");
        }

        check.reset();
    }else{
        STADIC_LOG(Severity::Error, "The parameter "+parameter+" is not found within the "+setName+" in "+m_SpaceName +".");
    }
    return false; // Add return for VS
}

}
