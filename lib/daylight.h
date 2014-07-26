#ifndef DAYLIGHT_H
#define DAYLIGHT_H

#include <QObject>

#include "stadicapi.h"

class STADIC_API Daylight : public QObject
{
    Q_OBJECT
public:
    explicit Daylight(QObject *parent = 0);
    bool simDaylight(QString fileName);

private:
    bool simBSDF(int blindGroupNum, int setting, QString radFileName, int BSDFNum, QString glassRad, QString bsdfRad);

signals:

public slots:

};

#endif // DAYLIGHT_H
