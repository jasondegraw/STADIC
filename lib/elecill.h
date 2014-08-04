#ifndef ELECILL_H
#define ELECILL_H

#include <QObject>
#include <QVector>

#include "stadicapi.h"

namespace stadic {

class STADIC_API ElecIll : public QObject
{
    Q_OBJECT
public:
    explicit ElecIll(QObject *parent = 0);
    bool parseIll(QString fileName);

    //Setters
    void setIlluminance(double value);
    void setX(QString x);
    void setY(QString y);
    void setZ(QString z);

    //Getters
    std::vector<double> illuminance();
    std::vector<QString> x();
    std::vector<QString> y();
    std::vector<QString> z();

private:
    std::vector<double> m_Illuminace;
    std::vector<QString> m_X;
    std::vector<QString> m_Y;
    std::vector<QString> m_Z;

signals:

public slots:

};

}

#endif // ELECILL_H
