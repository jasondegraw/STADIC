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
