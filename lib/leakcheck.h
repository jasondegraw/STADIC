#ifndef LEAKCHECK_H
#define LEAKCHECK_H

#include <QObject>
#include <QStringList>
#include <QPolygonF>
#include <QPointF>
#include <QDir>
#include <QTextStream>
#include <QProcess>
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
    bool setRadFile(QStringList files);
    bool setFloorLayers(QStringList layers);
    bool setX(double x);
    bool setY(double y);
    bool setZ(double z);
    bool setPoint(std::vector<double> point);
    bool setReflectance(int ref);
    bool setWorkingDirectory(QString wDir);

private:
    QStringList m_RadFiles;
    QStringList m_FloorLayers;
    std::vector<double> m_TestPoint;
    RadFileData m_RadGeo;
    std::vector<QPolygonF> m_Polygons;
    QPolygonF m_UnitedPolygon;
    double m_Reflectance;
    QDir m_Dir;
    QTextStream m_ErrorLog;
    QTextStream m_ErrorLog2;
    QTextStream m_Output2;
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
    void captureErros();
    void captureErrors2();
    void captureOutput2();

};

}

#endif // LEAKCHECK_H
