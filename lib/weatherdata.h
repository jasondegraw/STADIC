#ifndef MAKEWEA_H
#define MAKEWEA_H

#include <string>
#include <vector>

#include "stadicapi.h"

#ifdef _MSC_VER // Suppress warning C4251: http://support.microsoft.com/kb/168958/en-us
template class __declspec(dllexport) std::vector<int>;
template class __declspec(dllexport) std::vector<double>;
//template class __declspec(dllexport) std::string;
template class __declspec(dllexport) std::basic_string < char, std::char_traits<char>, std::allocator<char> >;
template class __declspec(dllexport) std::vector<std::string>;
#endif

namespace stadic {

class STADIC_API WeatherData
{
public:
    WeatherData();
    bool parseWeather(std::string file);
    bool writeWea(std::string file);

    //Setters
    /*
    void setMonth(int month);
    void setDay(int day);
    void setHour(double hour);
    void setDN(double dn);
    void setDH(double dh);
    */
    void setPlace(std::string place);
    void setLatitude(std::string lat);
    void setLongitude(std::string lon);
    void setTimeZone(std::string timeZone);
    void setElevation(std::string elev);

    //Getters
    std::vector<int> month() const;
    std::vector<int> day() const;
    std::vector<double> hour() const;
    std::vector<std::string> directNormal() const;
    std::vector<std::string> diffuseHorizontal() const;
    std::vector<double> directIlluminance() const;
    std::vector<double> dewPointC() const;
    std::string place() const;
    std::string latitude() const;
    std::string longitude() const;
    std::string timeZone() const;
    std::string elevation() const;
    std::vector<int> julianDate() const;

private:
    bool parseEPW(std::string file);
    bool parseTMY(std::string file);
    bool calcDirectIll();
    void setSolarPositions();
    double solarDec(int julianDate);
    double solarTimeAdj(int julianDate);
    double solarAlt(double solarDeclination, double time);
    double solarAz(double solarDeclination, double time);
    double solarZen(double solarAltAng);
    double degToRad(double val);
    void calcEpsilon();
    void calcDelta();
    void calcAPWC();
    int skyBin(double epsilon);
    std::vector<double> directLumEff(double epsilon);

    std::vector<int> m_Month;
    std::vector<int> m_Day;
    std::vector<double> m_Hour;
    std::vector<int> m_JulianDate;
    std::vector<std::string> m_DirectNormal;
    std::vector<double> m_DirectIlluminance;
    std::vector<std::string> m_DiffuseHorizontal;
    std::vector<double> m_DewPointC;
    std::vector<double> m_SolarDec;
    std::vector<double> m_SolarTimeAdj;
    std::vector<double> m_SolarAlt;
    std::vector<double> m_SolarAz;
    std::vector<double> m_SolarZenAng;
    std::vector<double> m_Epsilon;          //Sky's cleaness
    std::vector<double> m_Delta;            //Sky's brightness
    std::vector<double> m_APWC;             //Atmospheric preciptiable water content
    std::string m_Place;
    std::string m_Latitude;
    std::string m_Longitude;
    std::string m_TimeZone;
    std::string m_Elevation;

};

}

#endif // MAKEWEA_H
