#ifndef SHADECONTROL_H
#define SHADECONTROL_H

#include <string>
#include <vector>

#include <QJsonObject>
#include <boost/property_tree/ptree.hpp>

#include "stadicapi.h"

namespace stadic {

class STADIC_API ShadeControl
{
public:
    explicit ShadeControl();
    bool parseJson(const QJsonObject &object);
    bool parseJson (const boost::property_tree::ptree json);

    //Setters
    bool setMethod(std::string method);
    bool setElevationAzimuth(double value);
    bool setAngleSettings(double value);
    bool setLocation(double x, double y, double z, double xd, double yd, double zd, double spin);
    bool setSignalSettings(double value);
    bool setSensorType(std::string type);
    void setSensorFile(std::string file);

    //Getters
    std::string controlMethod();
    double elevationAzimuth();
    std::vector<double> angleSettings();
    std::vector<double> location();
    std::vector<double> signalSettings();
    std::string sensorType();
    std::string sensorFile();


private:
    std::string m_Method;
    double m_ElevationAzimuth;
    std::vector<double> m_AngleSettings;
    std::vector<double> m_Location;
    std::vector<double>m_SignalSettings;
    std::string m_SensorType;                                       //  Variable holding the sensor type
    std::string m_SensorFile;                                       //  Variable holding the sensor file
};

}

#endif // SHADECONTROL_H
