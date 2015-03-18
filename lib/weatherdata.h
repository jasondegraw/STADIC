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

#ifndef MAKEWEA_H
#define MAKEWEA_H

#include <string>
#include <vector>

#include "stadicapi.h"

namespace stadic {

class STADIC_API WeatherData
{
public:
    WeatherData();
    bool parseWeather(std::string file);                    //Function to parse the weather file
    bool writeWea(std::string file);                        //Function to write the wea file to a given filename

    //Setters
    /*
    void setMonth(int month);
    void setDay(int day);
    void setHour(double hour);
    void setDN(double dn);
    void setDH(double dh);
    */
    void setPlace(std::string place);                       //Function to set the place
    void setLatitude(std::string lat);                      //Function to set the latitude
    void setLongitude(std::string lon);                     //Function to set the longitude
    void setTimeZone(std::string timeZone);                 //Function to set the timezone
    void setElevation(std::string elev);                    //Function to set the elevation

    //Getters
    std::vector<int> month() const;                         //Function that returns the month per interval as a vector
    std::vector<int> day() const;                           //Function that returns the day per interval as a vector
    std::vector<double> hour() const;                       //Function that returns the hour per interval as a vector
    std::vector<std::string> directNormal() const;          //Function that returns the directNormal per interval as a vector
    std::vector<std::string> diffuseHorizontal() const;     //Function that returns the diffuseHorizontal per interval as a vector
    std::vector<double> directIlluminance() const;          //Function that returns the directIlluminance per interval as a vector
    std::vector<double> dewPointC() const;                  //Function that returns the dew point per interval as a vector
    std::string place() const;                              //Function that returns the place as a string
    std::string latitude() const;                           //Function that returns the latitude as a string
    std::string longitude() const;                          //Function that returns the longitude as a string
    std::string timeZone() const;                           //Function that returns the timezone as a string
    double timeZoneDeg() const;                             //Function that returns the timezone as a double in degrees
    std::string elevation() const;                          //Function that returns the elevation as a string
    std::vector<int> julianDate() const;                    //Function that returns the julian date as a vector

private:
    bool parseEPW(std::string file);                        //Function to parse an EPW file
    bool parseTMY(std::string file);                        //Function to pase a TMY file
    bool calcDirectIll();                                   //Function to calculate the direct illuminance
    void setSolarPositions();                               //Function to set the solar positions
    double solarDec(int julianDate);                        //Function to calculate the solar declination angle
    double solarTimeAdj(int julianDate);                    //Function to calculate the solar time adjustment
    double solarAlt(double solarDeclination, double time);  //Function to calculate the solar altitude angle
    double solarAz(double solarDeclination, double time);   //Function to calculate the solar azimuth angle
    double solarZen(double solarAltAng);                    //Function to calculate the soalr zenith angle with bounds
    double degToRad(double val);                            //Function to conver degrees to radians
    void calcEpsilon();                                     //Function to calculate the sky clearness
    void calcDelta();                                       //Function to calculate the sky brightness
    void calcAPWC();                                        //Function to calculate the atmospheric precipitable water content
    int skyBin(double epsilon);                             //Function to determine which bin epsilon fits into
    std::vector<double> directLumEff(double epsilon);       //Function that returns a vector of direct luminous efficiency multipliers

    std::vector<int> m_Month;                               //Vector holding the month per interval
    std::vector<int> m_Day;                                 //Vector holding the day per interval
    std::vector<double> m_Hour;                             //Vector holding the hour per interval
    std::vector<int> m_JulianDate;                          //Vector holding the julian date per interval
    std::vector<std::string> m_DirectNormal;                //Vector holding the direct normal as a string per interval
    std::vector<double> m_DirectIlluminance;                //Vector holding the direct illuminance as a double per interval
    std::vector<std::string> m_DiffuseHorizontal;           //Vector holding the diffuse horizontal as a string per interval
    std::vector<double> m_DewPointC;                        //Vector holding the dewpoint as a double per interval
    std::vector<double> m_SolarDec;                         //Vector holding the solar declination angle as a double per interval
    std::vector<double> m_SolarTimeAdj;                     //Vector holding the solar time adjustment as a double per interval
    std::vector<double> m_SolarAlt;                         //Vector holding the solar altitude angle as a double per interval
    std::vector<double> m_SolarAz;                          //Vector holding the solar azimuth angle as a double per interval
    std::vector<double> m_SolarZenAng;                      //Vector holding the solar zenith angle as a double per interval
    std::vector<double> m_Epsilon;                          //Vector holding the Sky's cleaness as a double per interval
    std::vector<double> m_Delta;                            //Vector holding the Sky's brightness as a double per interval
    std::vector<double> m_APWC;                             //Vector holding the Atmospheric preciptiable water content as a double per interval
    std::string m_Place;                                    //Variable holding the place as a string
    std::string m_Latitude;                                 //Variable holding the latitude as a string
    std::string m_Longitude;                                //Variable holding the longitude as a string
    std::string m_TimeZone;                                 //Variable holding the timezone as a string
    std::string m_Elevation;                                //Variable holding the elevation as a string

};

}

#endif // MAKEWEA_H
