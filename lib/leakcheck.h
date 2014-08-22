#ifndef LEAKCHECK_H
#define LEAKCHECK_H

#include <QObject>
#include <QStringList>
#include <QPolygonF>
#include <QPointF>
#include "stadicapi.h"
#include "radfiledata.h"
#include "radprimitive.h"

namespace stadic{

class LeakCheck : public QObject
{
    Q_OBJECT
public:
    explicit LeakCheck(QObject *parent = 0);
    bool isEnclosed();


//Setters
    bool setRadFile(QStringList files);
    bool setFloorLayers(QStringList layers);
    bool setX(double x);
    bool setY(double y);
    bool setZ(double z);
    bool setPoint(std::vector<double> point);

private:
    QStringList m_RadFiles;
    QStringList m_FloorLayers;
    std::vector<double> m_TestPoint;
    RadFileData m_RadGeo;
    std::vector<QPolygonF> m_Polygons;
    QPolygonF m_UnitedPolygon;

    bool unitePolygons();
    bool checkPoint();

signals:

public slots:

};

}

#endif // LEAKCHECK_H
