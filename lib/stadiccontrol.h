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

#ifndef STADICCONTROL_H
#define STADICCONTROL_H

#include <string>
#include <vector>
#include "windowgroup.h"
#include "controlzone.h"
#include <boost/property_tree/ptree.hpp>
#include <boost/optional/optional.hpp>
#include "logging.h"

#include "stadicapi.h"

namespace stadic {

class STADIC_API Control
{
public:
    Control();
    bool parseJson(std::string file);

    //Setters
    //******************
    //Folder Information
    //******************
    void setProjectName(std::string name);
    void setProjectFolder(std::string folder);
    void setTmpFolder(std::string folder);
    void setGeoFolder(std::string folder);
    void setIESFolder(std::string folder);
    void setResultsFolder(std::string folder);
    void setDataFolder(std::string folder);

    //******************
    //Site Information
    //******************
    bool setGroundReflect(double value);
    void setWeaDataFile(std::string file);
    bool setFirstDay(int value);


    //******************
    //Geometry Information
    //******************
    void setMatFile(std::string file);
    void setGeoFile(std::string file);
    bool setBuildingRotation(double value);
    void setPTSFile(std::string file);
    bool setImportUnits(std::string units);
    void setIllumUnits(std::string units);
    bool setDisplayUnits(std::string units);
    void setOccSchedule(std::string file);
    bool setTargetIlluminance(double value);

    //******************
    //Simulation Settings
    //******************
    bool setSunDivisions(int value);
    bool setSkyDivisions(int value);
    void setDaylightSavingsTime(bool value);
    bool setDefaultRadianceParameters();
    bool setAB(int value);
    bool setAD(int value);
    bool setAS(int value);
    bool setAR(int value);
    bool setAA(double value);
    bool setLR(int value);
    bool setST(double value);
    bool setSJ(double value);
    bool setLW(double value);
    bool setDJ(double value);
    bool setDS(double value);
    bool setDR(int value);
    bool setDP(double value);

    //******************
    //Metrics
    //******************
    bool setDA(bool run, double illum);
    bool setcDA(bool run, double illum);
    bool setsDA(bool run, double illum, double DAFrac, double startTime, double endTime);
    bool setOccsDA(bool run, double illum, double DAFrac);
    void setDF(bool run);
    bool setUDI(bool run, double minIllum, double maxIllum);


    //Getters
    //******************
    //Folder Information
    //******************
    std::string projectName();
    std::string projectFolder();
    std::string tmpFolder();
    std::string geoFolder();
    std::string iesFolder();
    std::string resultsFolder();
    std::string dataFolder();

    //******************
    //Site Information
    //******************
    double groundReflect();
    std::string weaDataFile();
    int firstDay();

    //******************
    //Geometry Information
    //******************
    std::string matFile();
    std::string geoFile();
    double buildingRotation();
    std::string ptsFile();
    std::vector<WindowGroup> windowGroups();
    std::string importUnits();
    std::string illumUnits();
    std::string displayUnits();
    std::string occSchedule();
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
    bool daylightSavingsTime();
    int ab();
    int ad();
    int as();
    int ar();
    double aa();
    int lr();
    double st();
    double sj();
    double lw();
    double dj();
    double ds();
    int dr();
    double dp();

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


    //******************
    //Folder Information
    //******************
    std::string m_ProjectName;                          //  Variable holding the project name
    std::string m_ProjectFolder;                        //  Variable holding the project folder
    std::string m_TmpFolder;                            //  Variable holding the tmp folder
    std::string m_GeoFolder;                            //  Variable holding the geometry folder
    std::string m_IESFolder;                            //  Variable holding the luminaire folder
    std::string m_ResultsFolder;                        //  Variable holding the results folder
    std::string m_DataFolder;                           //  Variable holding the data folder

    //******************
    //Site Information
    //******************
    double m_GroundReflect;                             //  Variable holding the ground reflectance
    std::string m_WeaDataFile;                          //  Variable holding the weather data file
    int m_FirstDay;                                     //  Variable holding the start day of the year

    //******************
    //Geometry Information
    //******************
    std::string m_MatFile;                              //  Variable holding the main material file
    std::string m_GeoFile;                              //  Variable holding the main geometry file
    double m_BuildingRotation;                          //  Variable holding the building rotation which is assumed to be positive=counter-clockwise
    std::string m_PTSFile;                              //  Variable holding the analysis grid file
    std::string m_ImportUnits;                          //  Variable holding the geometry file import units
    std::string m_IllumUnits;                           //  Variable holding the illuminance units
    std::string m_DisplayUnits;                         //  Variable holding the distance units for display
    std::string m_OccSchedule;                          //  Variable holding the occupancy schedule file
    double m_TargetIlluminance;                         //  Variable holding the target illuminance
    std::vector<WindowGroup> m_WindowGroups;

    //******************
    //Simulation Settings
    //******************
    int m_SunDivisions;                                 //  Variable holding the integer for the number of sun divisions
    int m_SkyDivisions;                                 //  Variable holding the integer for the number of sky divisions
    bool m_DaylightSavingsTime;                         //  Variable holding whether daylight savings time is enabled
    int m_AB;                                           //  Variable holding the number of ambient bounces
    int m_AD;                                           //  Variable holding the number of ambient divisions
    int m_AS;                                           //  Variable holding the number of ambient super-samples
    int m_AR;                                           //  Variable holding the ambient resolutions
    double m_AA;                                        //  Variable holding the ambient accuracy
    int m_LR;                                           //  Variable holding the maximum number of reflections
    double m_ST;                                        //  Variable holding the specular sampling threshold
    double m_SJ;                                        //  Variable holding the specular sampling jitter
    double m_LW;                                        //  Variable holding the minimum weight fraction to continue tracing a ray
    double m_DJ;                                        //  Variable holding the direct jitter fraction
    double m_DS;                                        //  Variable holding the direct sampling ratio
    int m_DR;                                           //  Variable holding the number of relays for secondary sources
    double m_DP;                                        //  Variable holding the secondary source presampling density

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
