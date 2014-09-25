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
    double illuminance();                               //Function to get the illuminance at the point described by x, y, z
    std::string x();                                    //Function to get the x location of the illuminance point
    std::string y();                                    //Function to get the y location of the illuminance point
    std::string z();                                    //Function to get the z location of the illuminance point

private:
    double m_Illuminance;                               //Variable holding the illuminance value
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

    //Setters
    //void setIlluminance(double value);
    //void setX(std::string x);
    //void setY(std::string y);
    //void setZ(std::string z);

    //Getters
    std::vector<SpatialIlluminance> illuminance();      //Function that returns the illuminance of an electric light zone as a vector of illuminance points

private:
    std::vector<SpatialIlluminance> m_Illuminance;      //Vector holding the illuminance points as SpatialIlluminance objects
};

}

#endif // ELECILL_H
