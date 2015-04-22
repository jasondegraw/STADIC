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

#ifndef SPACECONTROL_H
#define SPACECONTROL_H

#include <string>
#include <vector>
#include "windowgroup.h"
#include "controlzone.h"
#include <boost/optional.hpp>
#include "logging.h"
#include "buildingcontrol.h"

#include "stadicapi.h"

namespace stadic {

class STADIC_API Control
{
public:
    Control();
    bool parseJson(const JsonObject &json, BuildingControl *buildingControl);

    //Setters
    //******************
    //Folder Information
    //******************
    void setSpaceName(std::string name);
    void setSpaceDirectory(std::string directory);
    void setGeoDirectory(std::string directory);
    void setIESDirectory(std::string directory);
    void setResultsDirectory(std::string directory);
    void setInputDirectory(std::string directory);

    //******************
    //Site Information
    //******************
    bool setGroundReflect(double value);
    void setWeaFile(std::string fileName);

    //******************
    //Geometry Information
    //******************
    void setMatFile(std::string file);
    void setGeoFile(std::string file);
    void setBuildingRotation(double value);
    void setPTSFile(std::vector<std::string> files);
    void setXSpacing(std::string value);
    void setYSpacing(std::string value);
    void setOffset(std::string value);
    void setZOffset(std::string value);
    void setIdentifiers(std::vector<std::string> identifiers);
    void setModifiers(std::vector<std::string> modifiers);
    void setOccSchedule(std::string file);
    void setLightSchedule(std::string file);
    bool setTargetIlluminance(double value);

    //******************
    //Simulation Settings
    //******************
    bool setSunDivisions(int value);
    bool setSkyDivisions(int value);
    void setFirstDay(boost::optional<int> value);
    void setImportUnits(std::string units);
    void setIllumUnits(std::string units);
    void setDisplayUnits(std::string units);
    void setDaylightSavingsTime(bool DST);

    //******************
    //Metrics
    //******************
    bool setDA(bool run, double illum);
    void setCalcDA(bool run);
    bool setcDA(bool run, double illum);
    void setCalccDA(bool run);
    bool setsDA(bool run, double illum, double DAFrac, double startTime, double endTime);
    bool setsDAwgSettings(std::vector<int> settingNumbers);         //these are assumed to be base 1
    void setCalcsDA(bool run);
    bool setOccsDA(bool run, double illum, double DAFrac);
    void setCalcOccsDA(bool run);
    void setDF(bool run);
    bool setUDI(bool run, double minIllum, double maxIllum);
    void setCalcUDI(bool run);


    //Getters
    //******************
    //Folder Information
    //******************
    std::string spaceName();
    std::string spaceDirectory();
    std::string geoDirectory();
    std::string iesDirectory();
    std::string resultsDirectory();
    std::string inputDirectory();
    std::string intermediateDataDirectory();

    //******************
    //Site Information
    //******************
    double groundReflect();
    std::string weaFile();

    //******************
    //Geometry Information
    //******************
    std::string matFile();
    std::string geoFile();
    double buildingRotation();
    std::vector<std::string> ptsFile();
    boost::optional<std::string> xSpacing();
    boost::optional<std::string> ySpacing();
    boost::optional<std::string> offset();
    boost::optional<std::string> zOffset();
    boost::optional<std::vector<std::string>> identifiers();
    boost::optional<std::vector<std::string>> modifiers();
    std::vector<WindowGroup> windowGroups();
    std::string occSchedule();
    std::string lightSchedule();
    double targetIlluminance();

    //******************
    //Lighting Control
    //******************
    std::vector<ControlZone> controlZones();

    //******************
    //Simulation Settings
    //******************
    int sunDivisions();
    int skyDivisions();
    boost::optional<std::string> getRadParam(std::string parameterSet, std::string parameterName);
    boost::optional<std::unordered_map<std::string, std::string>> getParamSet(std::string setName);
    boost::optional<int> firstDay();
    std::string importUnits();
    std::string illumUnits();
    std::string displayUnits();
    bool daylightSavingsTime();


    //******************
    //Metrics
    //******************
    bool runDA();
    double DAIllum();
    bool runsDA();
    double sDAIllum();
    double sDAFrac();
    double sDAStart();
    double sDAEnd();
    std::vector<int> sDAwgSettings();
    bool runOccsDA();
    double occsDAIllum();
    double occsDAFrac();
    bool runcDA();
    double cDAIllum();
    bool runDF();
    bool runUDI();
    double UDIMin();
    double UDIMax();

private:
    bool verifyParameters();
    bool checkParameter(std::string setName, std::string parameter, std::string varType);

    //******************
    //Folder Information
    //******************
    std::string m_SpaceName;                          //  Variable holding the project name
    std::string m_SpaceDirectory;                     //  Variable holding the project folder
    std::string m_GeoDirectory;                       //  Variable holding the geometry folder
    std::string m_IESDirectory;                       //  Variable holding the luminaire folder
    std::string m_ResultsDirectory;                   //  Variable holding the results folder
    std::string m_InputDirectory;                     //  Variable holding the data folder

    //******************
    //Site Information
    //******************
    double m_GroundReflect;                             //  Variable holding the ground reflectance
    std::string m_WeaFile;                              //  Variable holding the weather file name

    //******************
    //Geometry Information
    //******************
    std::string m_MatFile;                              //  Variable holding the main material file
    std::string m_GeoFile;                              //  Variable holding the main geometry file
    double m_BuildingRotation;
    std::vector<std::string> m_PTSFile;                              //  Variable holding the analysis grid file
    boost::optional<std::string> m_XSpacing;
    boost::optional<std::string> m_YSpacing;
    boost::optional<std::string> m_Offset;
    boost::optional<std::string> m_ZOffset;
    boost::optional<std::vector<std::string>> m_Identifiers;
    boost::optional<std::vector<std::string>> m_Modifiers;

    std::string m_OccSchedule;                          //  Variable holding the occupancy schedule file
    std::string m_LightSchedule;
    double m_TargetIlluminance;                         //  Variable holding the target illuminance
    std::vector<WindowGroup> m_WindowGroups;

    //******************
    //Simulation Settings
    //******************
    int m_SunDivisions;                                 //  Variable holding the integer for the number of sun divisions
    int m_SkyDivisions;                                 //  Variable holding the integer for the number of sky divisions
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_RadParams;
    boost::optional<int> m_FirstDay;
    std::string m_ImportUnits;
    std::string m_IllumUnits;
    std::string m_displayUnits;
    bool m_DaylightSavingsTime;

    //******************
    //Lighting Control
    //******************
    std::vector<ControlZone> m_ControlZones;

    //******************
    //Metrics
    //******************
    bool m_DA;                                          //  Variable holding whether the DA analysis should be completed
    double m_DAIllum;                                   //  Variable holding the illuminance for the DA analysis
    bool m_sDA;                                         //  Variable holding whether the sDA analysis should be completed
    double m_sDAIllum;                                  //  Variable holding the illuminance for the sDA analysis
    double m_sDAFrac;                                   //  Variable holding the DA fraction for the sDA analysis
    double m_sDAStart;                                  //  Variable holding the start time for the sDA analysis
    double m_sDAEnd;                                    //  Variable holding the end time for the sDA analysis
    std::vector<int> m_sDAwgSettings;
    bool m_OccsDA;                                      //  Variable holding whether the occupancy schedule based sDA should be completed
    double m_OccsDAIllum;                               //  Variable holding the illuminance for the occupancy schedule based sDA analysis
    double m_OccsDAFrac;                                //  Variable holding the DA fraction for the occupancy schedule based sDA analysis
    bool m_cDA;                                         //  Variable holding whether the cDA analysis should be completed
    double m_cDAIllum;                                  //  Variable holding the illuminance for the cDA analysis
    bool m_DF;                                          //  Variable holding whether the DF analysis should be completed
    bool m_UDI;                                         //  Variable holding whether the UDI analysis shoud be completed
    double m_UDIMin;                                    //  Variable holding the minimum illuminance for UDI
    double m_UDIMax;                                    //  Variable holding the maximum illuminance for UDI
    
};

}

#endif // STADICCONTROL_H
