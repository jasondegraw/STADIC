#ifndef MAKEWEA_H
#define MAKEWEA_H

#include <QObject>
#include <vector>

class MakeWea : public QObject
{
    Q_OBJECT
public:
    explicit MakeWea(QObject *parent = 0);
    bool parseWeather(QString file);
    bool writeWea(QString file);

    //Setters
    void setMonth(int month);
    void setDay(int day);
    void setHour(double hour);
    void setDN(double dn);
    void setDH(double dh);
    void setPlace(QString place);
    void setLatitude(double lat);
    void setLongitude(double lon);
    void setTimeZone(double timeZone);
    void setElevation(double elev);

    //Getters
    std::vector<int> month();
    std::vector<int> day();
    std::vector<double> hour();
    std::vector<double> directNormal();
    std::vector<double> directHorizontal();
    QString place();
    double latitude();
    double longitude();
    double timeZone();
    double elevation();



private:
    bool parseEPW(QString file);
    bool parseTMY(QString file);

    std::vector<int> m_Month;
    std::vector<int> m_Day;
    std::vector<double> m_Hour;
    std::vector<double> m_DirectNormal;
    std::vector<double> m_DirectHorizontal;
    QString m_Place;
    double m_Latitude;
    double m_Longitude;
    double m_TimeZone;
    double m_Elevation;



signals:

public slots:

};

#endif // MAKEWEA_H
