#ifndef SHADECONTROL_H
#define SHADECONTROL_H

#include <QObject>
#include <vector>

#include "stadicapi.h"

namespace stadic {

class STADIC_API ShadeControl : public QObject
{
    Q_OBJECT
public:
    explicit ShadeControl(QObject *parent = 0);
    bool parseJson(const QJsonObject &object);

    //Setters
    bool setMethod(QString method);
    bool setElevationAzimuth(double value);
    bool setAngleSettings(double value);
    bool setLocation(double x, double y, double z, double xd, double yd, double zd, double spin);
    bool setSignalSettings(double value);
    bool setSensorType(QString type);
    void setSensorFile(QString file);

    //Getters
    QString controlMethod();
    double elevationAzimuth();
    std::vector<double> angleSettings();
    std::vector<double> location();
    std::vector<double> signalSettings();
    QString sensorType();
    QString sensorFile();


private:
    QString m_Method;
    double m_ElevationAzimuth;
    std::vector<double> m_AngleSettings;
    std::vector<double> m_Location;
    std::vector<double>m_SignalSettings;
    QString m_SensorType;                                       //  Variable holding the sensor type
    QString m_SensorFile;                                       //  Variable holding the sensor file

signals:
    
public slots:
    
};

}

#endif // SHADECONTROL_H
