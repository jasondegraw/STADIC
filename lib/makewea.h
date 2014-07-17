#ifndef MAKEWEA_H
#define MAKEWEA_H

#include <QObject>
#include <vector>

namespace stadic {

class WeatherData : public QObject
{
    Q_OBJECT
public:
    explicit WeatherData(QObject *parent = 0);
    bool parseWeather(QString file);
    bool writeWea(QString file);

    //Setters
    void setMonth(QString month);
    void setDay(QString day);
    void setHour(double hour);
    bool setDN(QString dn);
    bool setDH(QString dh);
    void setPlace(QString place);
    void setLatitude(QString lat);
    void setLongitude(QString lon);
    void setTimeZone(QString timeZone);
    void setElevation(QString elev);

    //Getters
    std::vector<QString> month() const;
    std::vector<QString> day() const;
    std::vector<double> hour() const;
    std::vector<QString> directNormal() const;
    std::vector<QString> directHorizontal() const;
    QString place() const;
    QString latitude() const;
    QString longitude() const;
    QString timeZone() const;
    QString elevation() const;

private:
    bool parseEPW(QString file);
    bool parseTMY(QString file);

    std::vector<QString> m_Month;
    std::vector<QString> m_Day;
    std::vector<double> m_Hour;
    std::vector<QString> m_DirectNormal;
    std::vector<QString> m_DirectHorizontal;
    QString m_Place;
    QString m_Latitude;
    QString m_Longitude;
    QString m_TimeZone;
    QString m_Elevation;



signals:

public slots:

};

}

#endif // MAKEWEA_H
