#ifndef DAYLIGHT_H
#define DAYLIGHT_H

#include <QObject>

class daylight : public QObject
{
    Q_OBJECT
public:
    explicit daylight(QObject *parent = 0);
    bool simDaylight(QString fileName);

private:
    bool simBSDF(int blindGroupNum, int setting, QString radFileName, int BSDFNum, QString glassRad, QString bsdfRad);

signals:

public slots:

};

#endif // DAYLIGHT_H
