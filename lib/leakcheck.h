#ifndef LEAKCHECK_H
#define LEAKCHECK_H

#include <QObject>
#include <QPolygonF>
#include <QPointF>
#include <QDir>
#include <QProcess>
#include <vector>
#include <string>
#include "stadicapi.h"
#include "radfiledata.h"
#include "radprimitive.h"

namespace stadic{

class STADIC_API LeakCheck : public QObject
{
    Q_OBJECT
public:
    explicit LeakCheck(QObject *parent = 0);
    bool isEnclosed();


//Setters
    bool setRadFile(std::vector<std::string> files);
    bool setFloorLayers(std::vector<std::string> layers);
    bool setX(double x);
    bool setY(double y);
    bool setZ(double z);
    bool setPoint(std::vector<double> point);
    bool setReflectance(int ref);
    bool setWorkingDirectory(std::string wDir);

private:
    std::vector<std::string> m_RadFiles;
    std::vector<std::string> m_FloorLayers;
    std::vector<double> m_TestPoint;
    RadFileData m_RadGeo;
    std::vector<QPolygonF> m_Polygons;
    QPolygonF m_UnitedPolygon;
    double m_Reflectance;
    QDir m_Dir;
    QProcess *m_Process;
    QProcess *m_Process2;

    bool unitePolygons();
    bool checkPoint();
    bool writePTS();
    bool writeExtraRad();
    bool xformModifiers();
    bool createOct();
    bool runCalc();


signals:

public slots:

private slots:


};

}

#endif // LEAKCHECK_H
