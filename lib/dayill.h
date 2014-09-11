#ifndef DAYILL_H
#define DAYILL_H

#include <QObject>
#include <QVector>

#include "stadicapi.h"

namespace stadic {

class STADIC_API TemporalIlluminance
{
public:
    TemporalIlluminance();
    TemporalIlluminance(int month, int day, double hour, const std::vector<double> &illuminance);

    bool add(std::vector<double> newIll);


    //Getters
    std::vector<double> illuminance();
    int month();
    int day();
    double hour();

private:
    std::vector<double> m_Illuminance;
    int m_Month;
    int m_Day;
    double m_Hour;
};


class STADIC_API DaylightIlluminanceData : public QObject
{
    Q_OBJECT
public:
    explicit DaylightIlluminanceData(QObject *parent = 0);
    bool parse(std::string fileName,std::string weaFile);
    bool parseTimeBased(std::string fileName);
    bool addIllFile(std::string fileName);
    bool addTimeBasedIll(std::string fileName);
    bool writeIllFile(std::string fileName);
    //Setters
    //void setIlluminance(double value);

    //Getters
    std::vector<TemporalIlluminance> illuminance();

    //int hoursGreaterThan(double value, int point);

private:
    std::vector<TemporalIlluminance> m_data;

signals:

public slots:

};

}

#endif // DAYILL_H
