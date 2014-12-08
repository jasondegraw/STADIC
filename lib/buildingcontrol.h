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

#ifndef BUILDINGCONTROL_H
#define BUILDINGCONTROL_H

#include <string>
#include <vector>
#include "spacecontrol.h"
#include <boost/optional.hpp>
#include "logging.h"
#include <unordered_map>

#include "stadicapi.h"

namespace stadic {

class STADIC_API BuildingControl
{
public:
    BuildingControl();
    bool parseJson(const std::string &file);

    //Setters

    //******************
    //General Information
    //******************
    bool setBuildingRotation(double value);
    void resetBuildingRotation();
    void setWeaDataFile(std::string file);
    void resetWeaDataFile();
    bool setFirstDay(int value);
    void resetFirstDay();
    bool setImportUnits(std::string units);
    void resetImportUnits();
    void setIllumUnits(std::string units);
    void resetIllumUnits();
    bool setDisplayUnits(std::string units);
    void resetDisplayUnits();
    bool setTargetIlluminance(double value);
    void resetTargetIlluminance();
    bool setSunDivisions(int value);
    void resetSunDivisions();
    bool setSkyDivisions(int value);
    void resetSkyDivisions();
    void setDaylightSavingsTime(bool value);
    void resetDaylightSavingsTime();
    bool setRadianceParameters(std::string radSet, std::string radParam, std::string radVal);

    //Getters
    //******************
    //Spaces
    //******************
    std::vector<Control> spaces();

    //******************
    //General
    //******************
    std::string weaDataFile();
    int firstDay();
    double buildingRotation();
    std::string importUnits();
    std::string illumUnits();
    std::string displayUnits();
    double targetIlluminance();
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


private:

    //******************
    //Spaces
    //******************
    std::vector<Control> m_Spaces;

    //******************
    //General Information
    //******************
    std::string m_WeaDataFile;
    int m_FirstDay;
    double m_BuildingRotation;
    std::string m_ImportUnits;                          //  Variable holding the geometry file import units
    std::string m_IllumUnits;                           //  Variable holding the illuminance units
    std::string m_DisplayUnits;                         //  Variable holding the distance units for display
    std::string m_OccSchedule;                          //  Variable holding the occupancy schedule file
    double m_TargetIlluminance;                         //  Variable holding the target illuminance
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
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_RadParams;

    //******************
    //Lighting Control
    //******************
    std::vector<ControlZone> m_ControlZones;
};

}

#endif // STADICCONTROL_H
