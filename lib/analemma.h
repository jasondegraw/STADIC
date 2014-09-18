#ifndef ANALEMMA_H
#define ANALEMMA_H
#include "weatherdata.h"
#include <string>
#include "stadicapi.h"
#include <vector>

/*
 * Adapted from Greg Ward's gendaymtx
 */

namespace stadic{

class STADIC_API Analemma
{
public:
    Analemma(std::string file);

    //Setters
    void setRotation(double val);
    void setMatFile(std::string file);
    void setGeoFile(std::string file);

    //Getters

    //Functions
    bool genSun();

private:
    //Variables
    std::string m_WeatherFile;
    WeatherData m_WeaData;
    std::vector<std::vector<double>> m_SunLoc;
    std::vector<std::vector<std::vector<double>>> m_SunVal;
    double m_Rotation;
    int m_numSuns;
    std::string m_MatFile;
    std::string m_GeoFile;
    std::vector<int> m_ClosestSun;

    //Functions
    bool parseWeather();
    bool getSunPos();
    std::vector<double> pos(double altitude, double azimuth);
    double degToRad(double val);
    double solarDec(int julianDate);
    double solarTimeAdj(int julianDate);
    double solarAlt(double solarDeclination, double time);
    double solarAz(double solarDeclination, double time);
    double dotProd(std::vector<double> vec1,std::vector<double> vec2);
    bool closestSun();
    bool genSunMtx();
};

}

#endif // ANALEMMA_H
