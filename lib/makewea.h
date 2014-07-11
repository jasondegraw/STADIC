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
    /*
    void setMonth(int month);
    void setDay(int day);
    void setHour(double hour);
    void setDN(double dn);
    void setDH(double dh);
    */
    void setPlace(QString place);
    void setLatitude(QString lat);
    void setLongitude(QString lon);
    void setTimeZone(QString timeZone);
    void setElevation(QString elev);

    //Getters
    std::vector<int> month();
    std::vector<int> day();
    std::vector<double> hour();
    std::vector<QString> directNormal();
    std::vector<QString> directHorizontal();
    QString place();
    QString latitude();
    QString longitude();
    QString timeZone();
    QString elevation();



private:
    bool parseEPW(QString file);
    bool parseTMY(QString file);

    std::vector<int> m_Month;
    std::vector<int> m_Day;
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

#endif // MAKEWEA_H
