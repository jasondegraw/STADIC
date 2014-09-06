#ifndef MAKEWEA_H
#define MAKEWEA_H

#include <string>
#include <vector>
#include <string>
#include "stadicapi.h"

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
    std::vector<std::string> directHorizontal() const;
    std::string place() const;
    std::string latitude() const;
    std::string longitude() const;
    std::string timeZone() const;
    std::string elevation() const;

private:
    bool parseEPW(std::string file);
    bool parseTMY(std::string file);

    std::vector<int> m_Month;
    std::vector<int> m_Day;
    std::vector<double> m_Hour;
    std::vector<std::string> m_DirectNormal;
    std::vector<std::string> m_DirectHorizontal;
    std::string m_Place;
    std::string m_Latitude;
    std::string m_Longitude;
    std::string m_TimeZone;
    std::string m_Elevation;

};

}

#endif // MAKEWEA_H
