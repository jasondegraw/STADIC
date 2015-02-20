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

#ifndef SHADECONTROL_H
#define SHADECONTROL_H

#include <string>
#include <vector>

#include "stadicapi.h"
#include "jsonobjects.h"

#ifdef _MSC_VER // Suppress warning C4251: http://support.microsoft.com/kb/168958/en-us
template class __declspec(dllexport) std::vector<double>;
template class __declspec(dllexport) std::basic_string < char, std::char_traits<char>, std::allocator<char> >;
#endif

namespace stadic {

class STADIC_API ShadeControl
{
public:
    explicit ShadeControl();
    bool parseJson(const JsonObject &json);

    //Setters
    bool setMethod(std::string method);
    bool setElevationAzimuth(double value);
    bool setAngleSettings(double value);
    bool setLocation(double x, double y, double z, double xd, double yd, double zd, double spin);
    bool setSignalSettings(double value);
    bool setSensorType(std::string type);
    void setSensorFile(std::string file);

    //Getters
    std::string controlMethod();
    double elevationAzimuth();
    std::vector<double> angleSettings();
    std::vector<double> location();
    std::vector<double> signalSettings();
    std::string sensorType();
    std::string sensorFile();


private:
    bool readAutoProf(const JsonObject &json, std::string method);
    bool readAutoSign(const JsonObject &json, std::string method);
    std::string m_Method;
    double m_ElevationAzimuth;
    std::vector<double> m_AngleSettings;
    std::vector<double> m_Location;
    std::vector<double>m_SignalSettings;
    std::string m_SensorType;                                       //  Variable holding the sensor type
    std::string m_SensorFile;                                       //  Variable holding the sensor file
};

}

#endif // SHADECONTROL_H
