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
#include <boost/optional.hpp>
#include "logging.h"
#include <unordered_map>

#include "stadicapi.h"

namespace stadic {

class Control;

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
    boost::optional<std::string> weaDataFile();
    boost::optional<int> firstDay();
    boost::optional<double> buildingRotation();
    boost::optional<std::string> importUnits();
    boost::optional<std::string> illumUnits();
    boost::optional<std::string> displayUnits();
    boost::optional<double> targetIlluminance();
    boost::optional<int> sunDivisions();
    boost::optional<int> skyDivisions();
    boost::optional<bool> daylightSavingsTime();
    boost::optional<std::string> getRadParam(std::string parameterSet, std::string parameterName);


private:

    //******************
    //Spaces
    //******************
    std::vector<Control> m_Spaces;

    //******************
    //General Information
    //******************
    boost::optional<std::string> m_WeaDataFile;
    boost::optional<int> m_FirstDay;
    boost::optional<double> m_BuildingRotation;
    boost::optional<std::string> m_ImportUnits;                          //  Variable holding the geometry file import units
    boost::optional<std::string> m_IllumUnits;                           //  Variable holding the illuminance units
    boost::optional<std::string> m_DisplayUnits;                         //  Variable holding the distance units for display
    boost::optional<double> m_TargetIlluminance;                         //  Variable holding the target illuminance
    boost::optional<int> m_SunDivisions;                                 //  Variable holding the integer for the number of sun divisions
    boost::optional<int> m_SkyDivisions;                                 //  Variable holding the integer for the number of sky divisions
    boost::optional<bool> m_DaylightSavingsTime;                         //  Variable holding whether daylight savings time is enabled
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> m_RadParams;
};

}

#endif // STADICCONTROL_H
