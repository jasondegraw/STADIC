#ifndef DAYILL_H
#define DAYILL_H

#include <QObject>
#include <QVector>

class DayIll : public QObject
{
    Q_OBJECT
public:
    explicit DayIll(QObject *parent = 0);
    bool parseIll(QString fileName);

    //Setters
    void setIlluminance(double value);
    bool setMonth(QString month);
    bool setDay(QString day);
    bool setHour(QString hour);

    //Getters
    std::vector<double> illuminance();
    std::vector<QString> month();
    std::vector<QString> day();
    std::vector<QString> hour();

private:
    std::vector<double> m_Illuminance;
    std::vector<QString> m_Month;
    std::vector<QString> m_Day;
    std::vector<QString> m_Hour;

signals:

public slots:

};

#endif // DAYILL_H
