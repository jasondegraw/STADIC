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

#ifndef CONTROLZONE_H
#define CONTROLZONE_H

#include <vector>
#include <string>
#include "jsonobjects.h"

#include "stadicapi.h"

#ifdef _MSC_VER // Suppress warning C4251: http://support.microsoft.com/kb/168958/en-us
//template class __declspec(dllexport) boost::optional<std::string>;
//template class __declspec(dllexport) std::vector<double>;
//template class __declspec(dllexport) std::string;
//template class __declspec(dllexport) std::basic_string < char, std::char_traits<char>, std::allocator<char> >;
//template class __declspec(dllexport) boost::optional_detail::aligned_storage<std::basic_string<char,std::char_traits<char>,std::allocator<char>>>;
template class __declspec(dllexport) std::vector<std::string>;
template class __declspec(dllexport) std::vector<int>;
template class __declspec(dllexport) std::vector<bool>;
template class __declspec(dllexport) std::vector<std::vector<std::string>>;
#endif

namespace stadic {

class STADIC_API ControlZone
{
public:
    ControlZone();
    bool parseJson (const JsonObject &json);
    
    //Setters
    void setName(const std::string &name);
    bool setOptimumMethod(const std::string &method);
    bool setCPMethod(const std::string &method);
    void setNumCPs(int value);
    void setTargetPercentage(double value);
    void setExcludedPoints(const std::string &file);
    bool setCriticalPoints(int point);
    void setMaximumBFSignal(double value);
    void setMinimumBFSignal(double value);
    void setOffSignal(double value);
    void setOnSignal(double value);
    void setSetpointSignal(double value);
    void setSignal(double value);
    void setDimmingLevel(double value);
    void setSignalToBFFile(const std::string &file);
    bool setSensorType(const std::string &type);
    void setSensorFile(const std::string &file);
    bool setSensorLocation(double x, double y, double z, double xd, double yd, double zd, double spin);
    bool setAlgorithm(const std::string &algorithm);
    void setIESFile(const std::string &file);
    bool setLLF(double value);
    bool setLampLumens(int value);
    bool setBallastType(const std::string &type);
    bool setBFMin(double value);
    bool setBFMax(double value);
    bool setWattsMax(double value);
    bool setWattsMin(double value);
    bool setBallastFactor(double value);
    bool setWatts(double value);
    bool setLuminaireLayout(double x, double y, double z, double rotation, double tilt, double spin);


    //Getters
    std::string name();
    std::string optimumMethod();
    std::string cpMethod();
    int numCPs();
    double targetPercentage();
    std::string excludedPoints();
    std::vector<int> criticalPoints();
    double maximumBFSignal();
    double minimumBFSignal();
    double offSignal();
    double setpointSignal();
    double onSignal();
    double signal();
    double dimmingLevel();
    std::string signalToBFFile();
    std::string sensorType();
    std::string sensorFile();
    std::vector<double> sensorLocation();
    std::string algorithm();
    std::string iesFile();
    double llf();
    int lampLumens();
    std::string ballastType();
    double bfMin();
    double bfMax();
    double wattsMax();
    double wattsMin();
    double ballastFactor();
    double watts();
    std::vector<std::vector<double> > luminaireLayout();

private:
    std::string m_Name;                                         //  Control zone name
    std::string m_OptimumMethod;                                    //  Variable holding the optimum control method
    std::string m_ControlMethod;                                    //  Variable holding the actual control method
    std::string m_CPMethod;                                         //  Variable holding the critical point method
    int m_NumCPs;                                               //  Variable holding the number of critical points to consider for the "auto" feature
    double m_TargetPercentage;                                  //  Variable holding the target percentage of illuminance from the controlled zone as a means of excluding analysis points
    std::string m_ExcludedPoints;                                   //  Variable holding the excluded points file name
    std::vector<int> m_CriticalPoints;                          //  Variable holding the critical points for the "user" feature
    double m_MaximumBFSignal;                                   //  Variable holding the signal at the maximum ballast factor
    double m_MinimumBFSignal;                                   //  Variable holding the signal at the minimum ballast factor
    double m_OffSignal;                                         //  Variable holding the signal at which to turn the lights off
    double m_SetpointSignal;                                    //  Variable holding the signal at the set point
    double m_OnSignal;                                          //  Variable holding the signal at which to turn the lights on
    double m_Signal;                                            //  Variable holding the signal for the "user 1" algorithm
    double m_DimmingLevel;                                      //  Variable holding the dimming level for the "user 1" algorithm
    std::string m_SignalToBFFile;                                   //  Variable holding the signal to ballast factor file for the "user 1" algorithm
    std::string m_SensorType;                                       //  Variable holding the sensor type
    std::string m_SensorFile;                                       //  Variable holding the sensor file
    std::vector<double> m_SensorLocation;                       //  Variable holding the sensor location
    std::string m_Algorithm;                                        //  Variable holding the control algorithm
    std::string m_IESFile;                                          //  Variable holding the IES file
    double m_LLF;                                               //  Variable holding the Light Loss Factor
    int m_LampLumens;                                           //  Variable holding the Lamp Lumens
    std::string m_BallastType;                                      //  Variable holding the Ballast Type
    double m_BFMin;                                             //  Variable holding the Minimum Ballast Factor
    double m_BFMax;                                             //  Variable holding the Maximum Ballast Factor
    double m_WattsMax;                                          //  Variable holding the Maximum Watts
    double m_WattsMin;                                          //  Variable holding the Minimum Watts
    double m_BallastFactor;                                     //  Variable holding the Ballast Factor
    double m_Watts;                                             //  Variable holding the Watts
    std::vector<std::vector<double> > m_LuminaireLayout;        //  Variable holding the luminaire positions
    
};

}

#endif // CONTROLZONE_H
