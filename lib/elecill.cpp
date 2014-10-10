/****************************************************************
 * Copyright © 2014, The Pennsylvania State University
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms,
 * with or without modification, are permitted for
 * personal and commercial purposes provided that the
 * following conditions are met:
 *
 * 1. Redistribution of source code must retain the
 *    above copyright notice, this list of conditions
 *    and the following Disclaimer.
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

#include "elecill.h"
#include "logging.h"
#include "functions.h"
#include <fstream>

namespace stadic {


SpatialIlluminance::SpatialIlluminance()
{
    m_Illuminance = 0;
    m_x = m_y = m_z = "0.0";
}

SpatialIlluminance::SpatialIlluminance(std::string x, std::string y, std::string z, double illuminance) : SpatialIlluminance()
{
    m_Illuminance = illuminance;
    bool ok;
    stadic::toDouble(x, &ok);
    if(ok) {
        m_x = x;
    } else {
        STADIC_ERROR("Bad spatial illuminance x value ("+x+")");
    }
    stadic::toDouble(y, &ok);
    if(ok) {
        m_y = y;
    } else {
        STADIC_ERROR("Bad spatial illuminance y value ("+y+")");
    }
    stadic::toDouble(z, &ok);
    if(ok) {
        m_z = z;
    } else {
        STADIC_ERROR("Bad spatial illuminance z value ("+z+")");
    }
}

//Getters
double SpatialIlluminance::lux(){
    return m_Illuminance;
}
double SpatialIlluminance::fc(){
    return m_Illuminance/10.764;
}

std::string SpatialIlluminance::x(){
    return m_x;
}
std::string SpatialIlluminance::y(){
    return m_y;
}
std::string SpatialIlluminance::z(){
    return m_z;
}


ElectricIlluminanceData::ElectricIlluminanceData()
{
}

//Getters
std::vector<SpatialIlluminance> ElectricIlluminanceData::illuminance(){
    return m_Illuminance;
}

bool ElectricIlluminanceData::parseIlluminance(std::string fileName){
    std::ifstream iFile;
    iFile.open(fileName);
    if (!iFile.is_open()){
        STADIC_ERROR("The opening of the illuminance file "+fileName+ " failed.");
        return false;
    }
    std::string line;

    while (std::getline(iFile, line)){
        std::vector<std::string> vals;
        vals=split(line, ' ');
        if(vals.size() != 4) {
            STADIC_ERROR("The illuminance file "+fileName +" does not contain 4 items per line.");
            return false;
        }
        m_Illuminance.push_back(SpatialIlluminance(vals[0], vals[1], vals[2], toDouble(vals[3])));
    }
    iFile.close();
    return true;
}

}
