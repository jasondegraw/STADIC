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
    double illuminance();
    std::string x();  // Should these return double?
    std::string y();
    std::string z();

private:
    double m_Illuminance;
    std::string m_x;
    std::string m_y;
    std::string m_z;
};

#ifdef _MSC_VER // Suppress warning C4251: http://support.microsoft.com/kb/168958/en-us
template class __declspec(dllexport) std::vector<SpatialIlluminance>;
#endif

class STADIC_API ElectricIlluminanceData
{
public:
    ElectricIlluminanceData();
    bool parseIlluminance(std::string fileName);

    //Setters
    //void setIlluminance(double value);
    //void setX(std::string x);
    //void setY(std::string y);
    //void setZ(std::string z);

    //Getters
    std::vector<SpatialIlluminance> illuminance();

private:
    std::vector<SpatialIlluminance> m_Illuminance;
};

}

#endif // ELECILL_H
