#ifndef ELECILL_H
#define ELECILL_H

#include <string>
#include <vector>

#include "stadicapi.h"

#ifdef _MSC_VER // Suppress warning C4251: http://support.microsoft.com/kb/168958/en-us
template class __declspec(dllexport) std::vector<double>;
//template class __declspec(dllexport) std::basic_string < char, std::char_traits<char>, std::allocator<char> >;
template class __declspec(dllexport) std::vector<std::string>;
#endif

namespace stadic {

class STADIC_API ElectricIlluminance
{
public:
    ElectricIlluminance();
    bool parseIlluminance(std::string fileName);

    //Setters
    //void setIlluminance(double value);
    //void setX(std::string x);
    //void setY(std::string y);
    //void setZ(std::string z);

    //Getters
    std::vector<double> illuminance();
    std::vector<std::string> x();
    std::vector<std::string> y();
    std::vector<std::string> z();

private:
    std::vector<double> m_Illuminance;
    std::vector<std::string> m_X;
    std::vector<std::string> m_Y;
    std::vector<std::string> m_Z;

};

}

#endif // ELECILL_H
