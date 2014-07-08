#ifndef SHADECONTROL_H
#define SHADECONTROL_H

#include <QObject>
#include <vector>

class ShadeControl : public QObject
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

    //Getters
    QString controlMethod();
    double elevationAzimuth();
    std::vector<double> angleSettings();
    std::vector<double> location();
    std::vector<double> signalSettings();


private:
    QString m_Method;
    double m_ElevationAzimuth;
    std::vector<double> m_AngleSettings;
    std::vector<double> m_Location;
    std::vector<double>m_SignalSettings;

signals:
    
public slots:
    
};

#endif // SHADECONTROL_H
