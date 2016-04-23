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
bool ElectricIlluminanceData::parseNonSpatialIlluminance(std::string illFileName, std::string ptsFileName, std::string outFileName, std::string units){
    //Open all necessary files
    std::ifstream illFile;
    illFile.open(illFileName);
    if (!illFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the illuminance file "+illFileName+" has failed.");
        return false;
    }

    std::ifstream ptsFile;
    ptsFile.open(ptsFileName);
    if (!ptsFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the points file "+ptsFileName+" has failed.");
        return false;
    }

    //Begin reading files to merge them into the spatial illuminance data.
    std::string illLine;
    std::string ptsLine;
    while (std::getline(illFile, illLine)){
        std::getline(ptsFile, ptsLine);
        std::vector<std::string> ptsVals;
        ptsVals=split(ptsLine, ' ');
        if (ptsVals.size()!=6){
            STADIC_LOG(Severity::Error, "The points file "+ptsFileName +" does not contain 6 items per line.");
            return false;
        }
        m_Illuminance.push_back(SpatialIlluminance(ptsVals[0], ptsVals[1], ptsVals[2], toDouble(illLine)));
    }

    //Close all necessary files
    illFile.close();
    ptsFile.close();

    //Write the illuminance file
    if (!writeIlluminanceFile(outFileName, units)){
        return false;
    }

    return true;
}

bool ElectricIlluminanceData::writeIlluminanceFile(std::string fileName, std::string units){
    std::ofstream outFile;
    outFile.open(fileName);
    if (!outFile.is_open()){
        STADIC_LOG(Severity::Error, "The opening of the output illuminance file named "+fileName+" has failed.");
        return false;
    }
    for (int i=0;i<m_Illuminance.size();i++){
        outFile<<m_Illuminance[i].x()<<" "<<m_Illuminance[i].y()<<" "<<m_Illuminance[i].z()<<" ";
        if (units=="lux"){
            outFile<<m_Illuminance[i].lux()<<std::endl;
        }else if (units=="fc"){
            outFile<<m_Illuminance[i].fc()<<std::endl;
        }else{
            STADIC_LOG(Severity::Error, "The units for writing an illumiance file must be \"lux\" or \"fc\".");
            return false;
        }
    }
    outFile.close();
    return true;
}


}
