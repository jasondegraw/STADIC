/******************************************************************************
 * Copyright (c) 2014, The Pennsylvania State University
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

#ifndef ANALEMMA_H
#define ANALEMMA_H
#include "weatherdata.h"
#include <string>
#include "stadicapi.h"
#include <vector>

/*
 * Adapted from Greg Ward's and Ian Ashdown's gendaymtx
 */

namespace stadic{

class STADIC_API Analemma
{
public:
    Analemma(std::string file);                                             //Constructor that takes the weather file

    //Setters
    void setRotation(double val);                                           //Function to set the rotation of the building(ex. 0=South, 90=East, -90=West)
    void setMatFile(std::string file);                                      //Function to set the output sun material filename
    void setGeoFile(std::string file);                                      //Function to set the output sun geometry filename
    void setSMXFile(std::string file);                                      //Function to set the output smx filename

    //Getters

    //Functions
    bool genSun();                                                          //Main function that generates the sun files

private:
    //Variables
    std::string m_WeatherFile;                                              //Variable holding the input weather file
    WeatherData m_WeaData;                                                  //WeatherData object
    std::vector<std::vector<double> > m_SunLoc;                              //Vector holding the sun locations
    //std::vector<std::vector<std::vector<double> > > m_SunVal;                 //Vector holding the sun luminance values
    double m_Rotation;                                                      //Variable holding the building rotation
    int m_numSuns;                                                          //Variable holding the number of suns (Maybe not needed)
    std::string m_MatFile;                                                  //Variable holding the sun mateterial filename
    std::string m_GeoFile;                                                  //Variable holding the sun geometry filename
    std::string m_SMXFile;                                                  //Variable holding the sun smx filename
    std::vector<int> m_ClosestSun;                                          //Vector holding which sun is closest at any given hour
    std::vector<std::string> temporarySun;

    //Functions
    bool parseWeather();                                                    //Function to parse the weather file.  Handled by WeatherData object
    bool getSunPos();                                                       //Function to generate the sun positions at every hour
    std::vector<double> pos(double altitude, double azimuth);               //Function to calculate the position give altitude and azimuth
    double degToRad(double val);                                            //Function that takes degrees as a double and outputs radians as double
    double degToRad(std::string val);                                       //Function that takes degrees as a string and outputs radians as double
    double solarDec(int julianDate);                                        //Function to calculate the solar declination angle
    double solarTimeAdj(int julianDate);                                    //Function to calculate the solar time adjustment based on julian date and location
    double solarAlt(double solarDeclination, double time);                  //Function to calculate the solar altitude angle
    double solarAz(double solarDeclination, double time);                   //Function to calculate the solar azimuth angle
    double dotProd(std::vector<double> vec1,std::vector<double> vec2);      //Function to calculate the dot product of two 3 dimensional vectors
    bool closestSun();                                                      //Function to find the closest sun
    bool genSunMtx();                                                       //Function to generate the sun matrix
};

}

#endif // ANALEMMA_H
