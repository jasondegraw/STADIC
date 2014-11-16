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

#ifndef ELECILL_H
#define ELECILL_H

#include <string>
#include <vector>

#include "stadicapi.h"

#ifdef _MSC_VER // Suppress warning C4251: http://support.microsoft.com/kb/168958/en-us
template class __declspec(dllexport) std::basic_string < char, std::char_traits<char>, std::allocator<char> >;
#endif

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

#ifdef _MSC_VER // Suppress warning C4251: http://support.microsoft.com/kb/168958/en-us
template class __declspec(dllexport) std::vector<SpatialIlluminance>;
#endif

class STADIC_API ElectricIlluminanceData
{
public:
    ElectricIlluminanceData();
    bool parseIlluminance(std::string fileName);        //Function to parse an electric lighting illuminance file given the filename

    //Getters
    std::vector<SpatialIlluminance> illuminance();      //Function that returns the illuminance of an electric light zone as a vector of illuminance points in lux

private:
    std::vector<SpatialIlluminance> m_Illuminance;      //Vector holding the illuminance points as SpatialIlluminance objects
};

}

#endif // ELECILL_H
