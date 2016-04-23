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

#ifndef ELECILL_H
#define ELECILL_H

#include <string>
#include <vector>

#include "stadicapi.h"

namespace stadic {

class STADIC_API SpatialIlluminance
{
public:
    SpatialIlluminance();
    SpatialIlluminance(std::string x, std::string y, std::string z, double illuminance);

    //Getters
    double lux();                                       //Function to get the illuminance at the point described by x, y, z in lux
    double fc();                                        //Function to get the illuminance at the point described by x, y, z in fc
    std::string x();                                    //Function to get the x location of the illuminance point
    std::string y();                                    //Function to get the y location of the illuminance point
    std::string z();                                    //Function to get the z location of the illuminance point

private:
    double m_Illuminance;                               //Variable holding the illuminance values (in lux)
    std::string m_x;                                    //Variable holding the x location of the illuminance point
    std::string m_y;                                    //Variable holding the y location of the illuminance point
    std::string m_z;                                    //Variable holding the z location of the illuminance point
};

class STADIC_API ElectricIlluminanceData
{
public:
    ElectricIlluminanceData();
    bool parseIlluminance(std::string fileName);        //Function to parse an electric lighting illuminance file given the filename
    bool parseNonSpatialIlluminance(std::string illFileName, std::string ptsFileName, std::string outFileName, std::string units);
    bool writeIlluminanceFile(std::string fileName, std::string units);

    //Getters
    std::vector<SpatialIlluminance> illuminance();      //Function that returns the illuminance of an electric light zone as a vector of illuminance points in lux

private:
    std::vector<SpatialIlluminance> m_Illuminance;      //Vector holding the illuminance points as SpatialIlluminance objects
};

}

#endif // ELECILL_H
